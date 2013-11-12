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
const int arm_r2 = 1;
const long sbx_uid = 20000;

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
		memcpy(data.chars, laddr, j + 1);
		ptrace(PTRACE_POKEDATA, child, addr + i * 4, data.val);
	}
}

void trace(pid_t traced_process) {
	long orig_eax, fpath, uid;
	int status;
	int toggle = 0;
	pid_t now;
	while(1) {
		now = waitpid(-1, &status, __WALL);
		
		if ((status >> 16 == PTRACE_EVENT_FORK) || (status >> 16 == PTRACE_EVENT_VFORK) || (status >> 16 == PTRACE_EVENT_CLONE)) {
			pid_t newpid;
			ptrace(PTRACE_GETEVENTMSG, now, NULL, (long) &newpid);
			printf("pid %d attached.\n", newpid);
			ptrace(PTRACE_SETOPTIONS, newpid, NULL, PTRACE_O_TRACEFORK | PTRACE_O_TRACEVFORK | PTRACE_O_TRACECLONE);
			ptrace(PTRACE_SYSCALL, now, NULL, NULL);
			continue;
		}
		
		if (now != traced_process) {
			orig_eax = getSysCallNo(now);
			
			if (orig_eax == __NR_open) {
				if(toggle == 0) {
					toggle = 1;
					fpath = ptrace(PTRACE_PEEKUSER, now, 4 * arm_r2, NULL);
					char *str = (char *)calloc(128, sizeof(char));
					getdata(now, fpath, str);
					printf("filepath: %s\n", str);
					int result = changepath(str);
                                        if(result > 0){
					  putdata(now, fpath, str);
					  printf("new filepath: %s\n", str);
                                        }
                                        free(str);
				}
				else {
					toggle = 0;
				}
			}
			
			/* else if (orig_eax == __NR_setuid32) { */
			/* 	if (toggle == 0) { */
			/* 		uid = ptrace(PTRACE_PEEKUSER, now, 4 * arm_r1, NULL); */
			/* 		printf("uid: %d\n", uid); */
			/* 		uid = sbx_uid; */
			/* 		printf("new uid: %d\n", uid); */
			/* 		ptrace(PTRACE_POKEUSER, now, 4 * arm_r1, uid); */
			/* 		toggle = 1; */
			/* 	} */
			/* 	else { */
			/* 		toggle = 0; */
			/* 	} */
			/* } */
		}
		else {
			if (WIFEXITED(status)) {
				break;
			}
		}
		
		ptrace(PTRACE_SYSCALL, now, NULL, NULL);
	}
}
