#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h> 

#define EABI		0xef000000
#define FIX_OABI	0x0ff00000
#define OABI		0x0f900000
#define FIX_SYS		0x000fffff

const int long_size = sizeof(long);
const int arm_r1 = 0;

long getSysCallNo(pid_t pid)
{
	long scno = 0;
	struct pt_regs regs;
	ptrace(PTRACE_GETREGS, pid, NULL, &regs);
	scno = ptrace(PTRACE_PEEKTEXT, pid, (void *)(regs.ARM_pc - 4), NULL);
	if(scno == 0) {
		return 0;
	}
	if (scno == EABI) {
		scno = regs.ARM_r7;
	} else {
		if ((scno & FIX_OABI) != OABI) {
			return -1;
		}
		/* 
		 * Fixup the syscall number 
		 */
		scno &= FIX_SYS;
	}
	return scno;
}

void getdata(pid_t child, long addr, char *str)
{   
	char *laddr;
	int i, j;
	union u {
		long val;
		char chars[long_size];
	}data;
	i = 0;
	j = 0;
	laddr = str;
	int done = 0;
	while(1) {
		data.val = ptrace(PTRACE_PEEKDATA, child, addr + i * 4, NULL);
		for (j=0; j<long_size; j++) {
			if (data.chars[j] == 0) {
				done = 1;
				break;
			}
		}
		if (done){
			memcpy(laddr, data.chars, j+1);
			break;
		}
		else {
			memcpy(laddr, data.chars, long_size);
			++i;
			laddr += long_size;
		}
	}
}

void putdata(pid_t child, long addr, char *str)
{   
	char *laddr;
	int i, j;
	union u {
		long val;
		char chars[long_size];
	}data;
	int len = strlen(str);
	i = 0;
	j = len/long_size;
	laddr = str;
	while(i < j) {
		memcpy(data.chars, laddr, long_size);
		ptrace(PTRACE_POKEDATA, child, addr + i * 4, data.val);
		++i;
		laddr += long_size;
	}
	j = len % long_size;
	if(j != 0) {
		memcpy(data.chars, laddr, j);
		data.chars[j] = 0;
		ptrace(PTRACE_POKEDATA, child, addr + i * 4, data.val);
	}
}

void trace(pid_t traced_process) {
	long orig_eax;
	int status;
	int toggle = 0;
	pid_t child;
	struct pt_regs regs;
	while(1) {
		wait(&status);
		if(WIFEXITED(status)) {
			break;
		}
		orig_eax = getSysCallNo(traced_process);
		if (orig_eax == __NR_fork) {
			if(toggle == 0) {
				toggle = 1;
			}
			else {
				ptrace(PTRACE_GETREGS, traced_process, NULL, &regs);
				child = regs.ARM_r0;
				toggle = 0;
				pid_t pchild = fork();
				if (pchild == 0) {
					printf("trace child's pid: %d\n", child);
					if(0 != ptrace(PTRACE_ATTACH, child, NULL, NULL))
					{
						printf("Trace process failed.\n");
						exit(0);
					}
					long c_orig_eax;
					int c_status;
					int c_toggle = 0;
					char* str;
					struct pt_regs c_regs;
					long fpath;
					while(1) {
						wait(&c_status);
						if(WIFEXITED(c_status)) {
							break;
						}
						c_orig_eax = getSysCallNo(child);
						if (c_orig_eax == __NR_open) {
							if(c_toggle == 0) {
								c_toggle = 1;
								fpath = ptrace(PTRACE_PEEKUSER, child, 4 * arm_r1, NULL);
								str = (char *)calloc(128, sizeof(char));
								getdata(child, fpath, str);
								printf("filepath: %s\n", str);
								char* newPath = changepath(str);
								printf("new filepath: %s\n", newPath);
								putdata(child, fpath, newPath);
							}
							else {
								c_toggle = 0;
							}
						}
						ptrace(PTRACE_SYSCALL, child, NULL, NULL);
					}
					exit(0);
				}
				else {
				}
			}
		}
		ptrace(PTRACE_SYSCALL, traced_process, NULL, NULL);
	}
}
