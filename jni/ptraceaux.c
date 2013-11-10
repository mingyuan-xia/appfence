/*
 * ptraceaux.c
 * This header hides the details about ptrace
 */

#include <sys/ptrace.h>
#include <sys/syscall.h>
#include <string.h>
#include <stdio.h>
#include "ptraceaux.h"

int ptrace_attach(pid_t pid)
{
	return ptrace(PTRACE_ATTACH, pid, NULL, NULL);
}

int ptrace_detach(pid_t pid)
{
	return ptrace(PTRACE_DETACH, pid, NULL, NULL);
}

#define WORD_SIZE 4

void ptrace_read_data(pid_t pid, void *buf, void *addr, int nbytes)
{
	/* standard three-variable control */
	int remaining = nbytes, copy, offset = 0;
	char *dst = (char *)buf, *src = (char *)addr;

	while (remaining > 0) {
		/* a word */
		long v = ptrace(PTRACE_PEEKDATA, pid, src + offset, NULL);
		copy = (remaining < WORD_SIZE ? remaining : WORD_SIZE);
		memcpy(dst + offset, &v, copy);
		offset += WORD_SIZE;
		remaining -= WORD_SIZE;
	}
}

int ptrace_strlen(pid_t pid, void *addr){
	int length = 0;
	while(1) {
		long v = ptrace(PTRACE_PEEKDATA, pid, addr + length, NULL);
		long test = 0xff000000;
		for(; test & v != 0; test >> 8, length++);
		if(test != 0) break;
	}
	return length;
}

void ptrace_write_data(pid_t pid, void *buf, void *addr, int nbytes)
{
}

#define EABI		0xef000000
#define FIX_OABI	0x0ff00000
#define OABI		0x0f900000
#define FIX_SYS		0x000fffff


int ptrace_get_syscall_nr(pid_t pid)
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
void ptrace_setopt(pid_t pid, int opt)
{
	ptrace(PTRACE_SETOPTIONS, pid, NULL, (void *)opt);
}

int handle_syscall(pid_t pid)
{
	long syscall_no =  ptrace_get_syscall_nr(pid);
	struct pt_regs regs;
	ptrace(PTRACE_GETREGS, pid, NULL, &regs);
	/* printf("pid %d : %d sysno \n", pid, (int)syscall_no); */
	switch(syscall_no){
		case __NR_open:
		{
			
			/* int len = ptrace_strlen(pid, addr); */
			char path[regs.ARM_r2 + 1];
			printf("r1: %ld, r2: %ld\n", regs.ARM_r1, regs.ARM_r2);
			ptrace_write_data(pid, path, (void*)regs.ARM_r1, regs.ARM_r2+1);
			printf("pid %d open: %s\n",pid, path);
			break;
		}
	}
	return 0;
}
