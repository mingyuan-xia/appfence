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

pid_t ptrace_process(pid_t pid){
	if (ptrace_attach(pid)) {
		return -1;
	}
	printf("%d tracing process %d\n", getpid(), pid);

	int status;

	while(pid > 0){
		/* printf("addr : %x\n", ptrace_tool.ptrace_get_syscall_nr); */
		long syscall_no =  ptrace_tool.ptrace_get_syscall_nr(pid);
	/* 	struct pt_regs regs; */
	/* 	ptrace(PTRACE_GETREGS, pid, NULL, &regs); */
		printf("pid %d : %d sysno \n", pid, (int)syscall_no);
		/* switch(syscall_no){ */
		/* 	case __NR_open: */
		/* 	{ */
				
		/* 		/1* int len = ptrace_strlen(pid, addr); *1/ */
		/* 		char path[regs.ARM_r2 + 1]; */
		/* 		printf("r1: %ld, r2: %ld\n", regs.ARM_r1, regs.ARM_r2); */
		/* 		/1* ptrace_write_data(pid, path, (void*)regs.ARM_r1, regs.ARM_r2+1); *1/ */
		/* 		printf("pid %d open: %s\n",pid, path); */
		/* 		break; */
		/* 	} */
		/* } */
		ptrace(PTRACE_SYSCALL, pid, NULL, NULL);
		pid = waitpid(pid, &status, __WALL);
	}
	ptrace_detach(pid);
	printf("%d exit\n", pid);
	return -1;
}
