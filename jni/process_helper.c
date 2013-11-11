/*
 * process_helper.c
 * Implementation of process.h
 */
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <signal.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "process_helper.h"

pid_t ptrace_process(pid_t pid){
	printf("tracing process %d\n", pid);
	//TODO:
	return 0;
}

