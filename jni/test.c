/*
 * test.c
 * For unit test
 */

#include "zygote_helper.h"
#include "process_helper.h"
#include "ptraceaux.h"
#include <sys/wait.h>
#include <sys/ptrace.h>
#include <signal.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
	init_ptrace_tool(ARCH_ARM);
	pid_t pid = ptrace_zygote(zygote_find_process());
	if(pid > 0) {
		ptrace_process(pid);
	}
	/* int status; */
	/* printf("msg from the child %d\n", waitpid(pid, &status, __WALL)); */
	/* ptrace(PTRACE_CONT, pid, NULL, NULL); */
	/* ptrace(PTRACE_DETACH, pid, NULL, NULL); */
	return 0;
}

