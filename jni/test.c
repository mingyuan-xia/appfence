/*
 * test.c
 * For unit test
 */

#include "zygote_helper.h"
#include <sys/wait.h>
#include <sys/ptrace.h>
#include <signal.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
	pid_t pid = ptrace_zygote(zygote_find_process());
	int status;
	printf("msg from the child %d\n", waitpid(pid, &status, __WALL));
	ptrace(PTRACE_CONT, pid, NULL, NULL);
	ptrace(PTRACE_DETACH, pid, NULL, NULL);
	return 0;
}

