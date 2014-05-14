/*
 * ptraceaux.c
 * This header hides the details about ptrace
 */

#include <sys/ptrace.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <string.h>
#include <stdio.h>
#include "ptraceaux.h"

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

int ptrace_strlen(pid_t pid, tracee_ptr_t addr)
{
	int length = 0;
	while(1) {
		unsigned long v = ptrace(PTRACE_PEEKDATA, pid, addr + length, NULL);
		unsigned long test = 0x000000ff;
		for(; (test & v) != 0; test <<= 8){
			length++;
		}
		if(test != 0) break;
	}
	return length;
}

void ptrace_write_data(pid_t pid, void *buf, void *addr, int nbytes)
{
	int remaining = nbytes, copy, offset = 0;
	char *dst = (char *)addr, *src = (char *)buf;
	while (remaining > 0) {
		long v;
		copy = (remaining < WORD_SIZE ? remaining : WORD_SIZE);
		if(copy < WORD_SIZE){
			/* avoid destroy the memory after dst */
			v = ptrace(PTRACE_PEEKDATA, pid, dst + offset, NULL);
			memcpy(&v, (void *)(src + offset), copy);
		} else {
			memcpy(&v, (void *)(src + offset), copy);
		}

		//this may cause some problem
		//it always write back a word
		ptrace(PTRACE_POKEDATA, pid, (void *)(dst + offset), (void *)v);
		offset += WORD_SIZE;
		remaining -= WORD_SIZE;
	}
}

// ARM tool implementation

#define EABI		0xef000000
#define FIX_OABI	0x0ff00000
#define OABI		0x0f900000
#define FIX_SYS		0x000fffff

long ptrace_get_syscall_nr(pid_t pid)
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

long ptrace_get_syscall_arg(pid_t pid, int n)
{
	struct pt_regs regs;
	ptrace(PTRACE_GETREGS, pid, NULL, &regs);
	switch(n) {
		case 0:
			return regs.ARM_r0;
		case 1:
			return regs.ARM_r1;
		case 2:
			return regs.ARM_r2;
		case 3:
			return regs.ARM_r3;
		default:
			return -1;
	}
}

