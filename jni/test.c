/*
 * test.c
 * For unit test
 */

#include "zygote_helper.h"
#include "ptraceaux.h"
#include "sandbox_helper.h"
#include <sys/wait.h>
#include <sys/ptrace.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[])
{
	init_ptrace_tool(ARCH_ARM);
	int sandbox = 0;
	if(argc > 1 && strcmp(argv[1],"-o") == 0){
		sandbox = 1;
		printf("***************\nsandbox on!!\n***************\n");
	} else {
		printf("***************\nsandbox off! (-o to turn on)\n***************\n");
	}
	pid_t pid = ptrace_zygote(zygote_find_process());
	if(pid > 0) {
		ptrace_app_process(pid, sandbox);
	}
	/* int status; */
	/* printf("msg from the child %d\n", waitpid(pid, &status, __WALL)); */
	/* ptrace(PTRACE_CONT, pid, NULL, NULL); */
	/* ptrace(PTRACE_DETACH, pid, NULL, NULL); */
	return 0;
}

