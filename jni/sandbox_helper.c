/*
 * process_helper.c
 * Implementation of process_helper.h
 */
#include <sys/ptrace.h>
#include <sys/syscall.h>

#include <sys/wait.h>
#include <signal.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "sandbox_helper.h"
#include "ptraceaux.h"

pid_t ptrace_app_process(pid_t pid){
	if (ptrace_attach(pid)) {
		return -1;
	}
	printf("%d tracing process %d\n", getpid(), pid);

	int status;

	while(pid > 0){
		//syscall enter
		long syscall_no =  ptrace_tool.ptrace_get_syscall_nr(pid);
		switch(syscall_no){
			case __NR_open:
			{
				long arg0 = ptrace_tool.ptrace_get_syscall_arg(pid, 0);
				int len = ptrace_strlen(pid, (void*) arg0);
				char path[len + 1];
				ptrace_tool.ptrace_read_data(pid, path, (void *)arg0, len);
				path[len] = 0;
				/* ptrace_write_data(pid, path, (void*)regs.ARM_r1, regs.ARM_r2+1); */
				printf("pid %d open: %s\n",pid, path);
				break;
			}
		}
		ptrace(PTRACE_SYSCALL, pid, NULL, NULL);
		pid = waitpid(pid, &status, __WALL);
		//syscall return
		//TODO

		ptrace(PTRACE_SYSCALL, pid, NULL, NULL);
		pid = waitpid(pid, &status, __WALL);
	}
	ptrace_detach(pid);
	printf("%d exit\n", pid);
	return -1;
}
