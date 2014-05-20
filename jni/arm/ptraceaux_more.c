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

void ptrace_read_data(pid_t pid, void *buf, tracee_ptr_t addr, int nbytes)
{
	/* standard three-variable control */
	int remaining = nbytes, copy, offset = 0;
	char *dst = (char *)buf;
	tracee_ptr_t *src = addr;

	while (remaining > 0) {
		/* a tracer word */
		tracer_word_t v = ptrace(PTRACE_PEEKDATA, pid, src + offset, NULL);
		copy = (remaining < TRACER_WORD_SIZE ? remaining : TRACER_WORD_SIZE);
		memcpy(dst + offset, &v, copy);
		offset += copy;
		remaining -= copy;
	}
}

int ptrace_strlen(pid_t pid, tracee_ptr_t addr)
{
	// TODO refactoring away platform stickness
	int length = 0;
	while (1) {
		tracer_word_t v = ptrace(PTRACE_PEEKDATA, pid, addr + length, NULL);
		tracer_word_t test = 0x000000ff;
		for (; (test & v) != 0; test <<= 8){
			length++;
		}
		if (test != 0) break;
	}
	return length;
}

void ptrace_write_data(pid_t pid, void *buf, tracee_ptr_t addr, int nbytes)
{
	int remaining = nbytes, copy, offset = 0;
	tracee_ptr_t dst = addr; 
	char *src = (char *)buf;
	while (remaining > 0) {
		tracer_word_t v;
		copy = (remaining < TRACER_WORD_SIZE ? remaining : TRACER_WORD_SIZE);
		if(copy < TRACER_WORD_SIZE){
			/* save a few bytes beyond the scope 
			   TODO this might be problamtic
			   when these padding bytes go accorss the page boundary to an
			   unmapped page 
			*/
			v = ptrace(PTRACE_PEEKDATA, pid, dst + offset, NULL);
		}
		memcpy(&v, (void *)(src + offset), copy);
		ptrace(PTRACE_POKEDATA, pid, (void *)(dst + offset), (void *)v);
		offset += copy;
		remaining -= copy;
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

