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

#define APP_FLAG "app"
#define ZYGOTE_FLAG "zygote"

int main(int argc, char *argv[])
{
	init_ptrace_tool(ARCH_ARM);
	if(create_link(SANDBOX_STORAGE_PATH, SANDBOX_LINK) < 0) {
		printf("warnning: link existed\n");
	}
	printf("%d\n",argc);
	if(argc == 1) {
		pid_t pid = ptrace_zygote(zygote_find_process());
		if(pid > 0) {
			ptrace_app_process(pid, SANDBOX_ENABLED);
		}
		return 0;
	} else {
		if(strcmp(argv[1], APP_FLAG) == 0 && argc >= 2) {
			pid_t pid = atoi(argv[2]);
			if (ptrace_attach(pid)) {
				printf("Error: ptrace pid %d failed\n", pid);
				return -1;
			}
			ptrace_app_process(pid, SANDBOX_ENABLED);
			return 0;
		} else if(strcmp(argv[1], ZYGOTE_FLAG) == 0) {
			pid_t pid = ptrace_zygote(zygote_find_process());
			if(pid > 0) {
				ptrace_app_process(pid, SANDBOX_ENABLED);
			}
			return 0;
		}
	}
	printf("usage: appfence [%s pid] | [%s]\n", APP_FLAG, ZYGOTE_FLAG);
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
