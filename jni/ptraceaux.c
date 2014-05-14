#include "config.h"

/* Common part */
#include <sys/ptrace.h>
#include <sys/syscall.h>
#include <sys/types.h>
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

void ptrace_setopt(pid_t pid, int opt)
{
	ptrace(PTRACE_SETOPTIONS, pid, NULL, (void *)opt);
}

void ptrace_cont(pid_t pid)
{
	ptrace(PTRACE_SYSCALL, pid, NULL, NULL);
}

#ifdef SANDBOX_ARCH_ARM
  #include "arm/ptraceaux_more.c"
#endif /* ARM */

#ifdef SANDBOX_ARCH_X86
  #include "x86/ptraceaux_more.c"
#endif /* x86 */

