#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <sys/syscall.h>

#define EABI		0xef000000
#define FIX_OABI	0x0ff00000
#define OABI		0x0f900000
#define FIX_SYS		0x000fffff

long getSysCallNo(int pid)
{
	long scno = 0;
	struct pt_regs regs;
	ptrace(PTRACE_GETREGS, pid, NULL, &regs);
	scno = ptrace(PTRACE_PEEKTEXT, pid, (void *)(regs.ARM_pc - 4), NULL);
	if(scno == 0)
		return 0;
		/* Handle the EABI syscall convention.  We do not 
		   bother converting structures between the two 
		   ABIs, but basic functionality should work even 
		   if strace and the traced program have different 
		   ABIs.  */
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

int main(int argc, char *argv[])
{
	if(argc != 2) {
		printf("Usage: %s <pid to be traced>\n", argv[1]);
		return 1;
	}
																					   
	pid_t traced_process;
	int status;
	long sys;
	traced_process = atoi(argv[1]);
	if(0 != ptrace(PTRACE_ATTACH, traced_process, NULL, NULL))
	{
		printf("Trace process failed:%d.\n", errno);
		return 1;
	}
	while(1)
	{
		wait(&status);
		if(WIFEXITED(status))
		{
			break;
		}
		sys = getSysCallNo(traced_process);
		printf("syscall: %ld\n", sys);
		ptrace(PTRACE_SYSCALL, traced_process, NULL, NULL);
	}
																								   
	ptrace(PTRACE_DETACH, traced_process, NULL, NULL);
																								   
	return 0;
}
