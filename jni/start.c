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
				ptrace(PTRACE_GETREGS, traced_process, NULL, &regs);
				child = regs.ARM_r0;
				printf("1.child's pid: %d\n", child);
			}
			else {
				ptrace(PTRACE_GETREGS, traced_process, NULL, &regs);
				child = regs.ARM_r0;
				printf("2.child's pid: %d\n", child);
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
					struct pt_regs c_regs;
					while(1) {
						wait(&c_status);
						if(WIFEXITED(c_status)) {
							break;
						}
						c_orig_eax = getSysCallNo(child);
						printf("c_orig_eax: %d\n", c_orig_eax);
						if (c_orig_eax == __NR_setuid32) {
							if(c_toggle == 0) {
								c_toggle = 1;
								ptrace(PTRACE_GETREGS, child, NULL, &c_regs);
								printf("uid: %d\n", c_regs.ARM_r0);
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

int main(int argc, char *argv[])
{
	pid_t traced_process;
	if (strcmp(argv[1], "-p") == 0) {
		traced_process = atoi(argv[2]);	
		if(0 != ptrace(PTRACE_ATTACH, traced_process, NULL, NULL))
		{
			printf("Trace process failed.\n");
			return 1;
		}
		trace(traced_process);
	}
	return 0;
}
