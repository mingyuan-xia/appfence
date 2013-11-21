/*
 * test.c
 * For unit test
 */

#include "config.h"
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
	pid_t pid = ptrace_zygote(zygote_find_process());
	if(pid > 0) {
		ptrace_app_process(pid, SANDBOX_ENABLED);
	}
	return 0;
}

void blind_cont(pid_t pid) {
	int status;
	ptrace_setopt(pid, 0);
	while (1) {
		/* wait for all child threads/processes */
		int pid = waitpid(-1, &status, __WALL);
		printf("app pid=%d, status %x\n", pid, status);
		ptrace(PTRACE_CONT, pid, NULL, NULL);
		ptrace_detach(pid);
	}

}
