/*
 * zygote_finder.c
 * Implementation of zygote_finder.h
 */
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <signal.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "zygote_helper.h"
#include "ptraceaux.h"

#define DEFAULT_ZYGOTE_PID 37

pid_t zygote_find_process(void)
{
	char filepath[128];
	char buf[512], *p;
	FILE *f;
	pid_t pid = DEFAULT_ZYGOTE_PID;
	snprintf(filepath, 128, "/proc/%d/status", pid);
	f = fopen(filepath, "r");
	if (!f) {
		return -1;
	}
	/* first line is enough */
	fgets(buf, 512, f);
	fclose(f);
	if (strncmp(buf, "Name:", 5)) {
		return -1;
	}
	/* retrieve the name */
	*(strchr(buf, '\n')) = '\0';
	for (p = &buf[5]; *p != '\0'&& (*p == ' ' || *p == '\t'); ++p) ;
	if (!strncmp(p, "zygote", 6)) {
		return pid;
	}
	/* TODO: should be a loop for all processes in the /proc/ */
	return pid;
}

/* from the man page of ptrace */
#define IS_FORK_EVENT(status) (status>>8 == (SIGTRAP | (PTRACE_EVENT_FORK<<8)))
#define IS_VFORK_EVENT(status) (status>>8 == (SIGTRAP | (PTRACE_EVENT_VFORK<<8)))
#define IS_CLONE_EVENT(status) (status>>8 == (SIGTRAP | (PTRACE_EVENT_CLONE<<8)))

pid_t ptrace_zygote(pid_t zygote_pid)
{
	int status, ev;

	/* keep an eye on zygote */
	if (ptrace_attach(zygote_pid)) {
		printf("Error: ptrace zygote failed\n");
		return -1;
	}
	/* note that we set O_TRACEFORK to automatically ptrace the child forked by zygote */
	ptrace_setopt(zygote_pid, PTRACE_O_TRACEFORK | PTRACE_O_TRACEVFORK | PTRACE_O_TRACECLONE);

	while (1) {
		/* wait until zygote sends a signal */
		/* assert(zygote_pid == waitpid(zygote_pid, &status, __WALL)); */
		int pid = waitpid(-1, &status, __WALL);
		printf("pid: %d, status %x\n", pid, status);
		/* retrieve the event header */
		if(pid == zygote_pid){
			if (IS_FORK_EVENT(status) || IS_VFORK_EVENT(status) || IS_CLONE_EVENT(status)) {
				printf("ev from %d\n", zygote_pid);
				pid_t newpid;
				/* get the forked child pid from the msg */
				ptrace(PTRACE_GETEVENTMSG, zygote_pid, NULL, &newpid);
				/* the child is already ptraced since we have PTRACE_O_TRACEFORK */
				printf("zygote forks %d\n", newpid);
				/* let zygote continue and go*/
				ptrace(PTRACE_SYSCALL, pid, NULL, NULL);
				ptrace_detach(zygote_pid);
				return newpid;
			}
			ptrace(PTRACE_SYSCALL, pid, NULL, NULL);
		} else if (pid > 0){
			/* appfence detaches zygote and returns the app pid */
			printf("app process sneaks in\n");
			ptrace(PTRACE_SYSCALL, pid, NULL, NULL);
		} else {
			printf("Error: unexpected events\n");
			break;
		}
	}
	return -1;
}


