/*
 * ptraceaux.c
 * This header hides the details about ptrace
 */

#include <sys/ptrace.h>
#include <string.h>
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

void ptrace_write_data(pid_t pid, void *buf, void *addr, int nbytes)
{
}
/*
int ptrace_get_syscall_nr(pid_t pid)
{

}
*/
void ptrace_setopt(pid_t pid, int opt)
{
	ptrace(PTRACE_SETOPTIONS, pid, NULL, (void *)opt);
}
