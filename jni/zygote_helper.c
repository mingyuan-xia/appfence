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
#include <dirent.h>
#include <fnmatch.h>
#include "zygote_helper.h"
#include "ptraceaux.h"

#define DEFAULT_ZYGOTE_PID 37

static int is_the_zygote_process(pid_t pid)
{
	char filepath[128];
	char buf[512], *p;
	FILE *f;
	snprintf(filepath, sizeof(filepath), "/proc/%d/status", pid);
	f = fopen(filepath, "r");
	if (!f) {
		return 0;
	}

	int term1 = 0, term2 = 0;
	while (fgets(buf, sizeof(buf), f)) {
		if (strncmp(buf, "Name:", 5) == 0) {
			/* retrieve the name */
			*(strchr(buf, '\n')) = '\0';
			for (p = &buf[5]; *p != '\0'&& (*p == ' ' || *p == '\t'); ++p) ;
			if (!strncmp(p, "zygote", 6)) {
				term1 = 1;
			}
		}
		else if (strncmp(buf, "PPid:", 5) == 0) {
			/* retrieve ppid */
			for (p = &buf[5]; *p != '\0' && (*p == ' ' || *p == '\t'); ++p) ;
			if (atoi(p) == 1) {
				term2 = 1;
			}
		}

		if (term1 && term2) {
			break;
		}
	}
	fclose(f);
	return (term1 && term2);
}

pid_t zygote_find_process(void)
{
	char buf[1024];
	char path_buf[256];
	struct dirent *dp;
	DIR *dir;
	pid_t zygote_pid = DEFAULT_ZYGOTE_PID;

	dir = opendir("/proc");
	if (!dir)
		return zygote_pid;

	while ((dp = readdir(dir)) != NULL) {
		pid_t pid;
		int len;
		if (dp->d_type != DT_DIR)
			continue;

		const char *d = dp->d_name;
		while (*d != '\0' && isdigit(*d)) { d++; }
		if (*d != '\0')
			continue;

		snprintf(path_buf, sizeof(path_buf), "/proc/%s/exe", dp->d_name);
		len = readlink(path_buf, buf, sizeof(buf) - 1);
		if (len <= 0) {
			// printf("process info is null:%s\n", buf2);
			continue;
		}

		buf[len] = 0;
		if (fnmatch(buf, "/system/bin/app_process", FNM_PATHNAME) != 0) {
			// printf("process info is mismatch:%s\n", buf);
			continue;
		}

		pid = atoi(dp->d_name);
		if (is_the_zygote_process(pid)) {
			zygote_pid = pid;
			break;
		}
	}
	closedir(dir);
	return zygote_pid;
}

pid_t ptrace_zygote(pid_t zygote_pid)
{
	/**
	 * Zygote's behaivor is well understood. It calls fork() to make a new app process.
	 * The child app process then triggers the DalvikVM loading, etc.
	 */
	int status, ev;

	/* keep an eye on zygote */
        printf("zygote pid:%d\n",zygote_pid);
	if (ptrace_attach(zygote_pid)) {
		printf("Error: ptrace zygote failed\n");
		return -1;
	}

	/* use this option to automatically attach to the child forked by zygote */
	ptrace_setopt(zygote_pid, PTRACE_O_TRACEFORK | PTRACE_O_TRACECLONE | PTRACE_O_TRACESYSGOOD);

	while (1) {
		/* wait until zygote is trapped by a ptrace stop */
		/* int pid = waitpid(zygote_pid, &status, __WALL); */
		int pid = waitpid(-1, &status, __WALL);
		/* check if waitpid fails */
		if (pid == -1) {
			ptrace_detach(zygote_pid);
			printf("Error: ptrace zygote waitpid failed\n");
			return -1;
		}
		/* assert(zygote_pid == pid); */
		printf("pid: %d, status %x\n", pid, status);
		/* check if the fork() event happens */
		if (IS_FORK_EVENT(status)) {
			pid_t newpid;
			/* get the forked child pid from the msg */
			ptrace(PTRACE_GETEVENTMSG, zygote_pid, NULL, &newpid);
			/* the child is already ptraced since we have PTRACE_O_TRACEFORK */
			printf("zygote forks %d\n", newpid);
			/* let zygote continue and go */
			ptrace_detach(zygote_pid);
			if(fork() == 0) {
				if(ptrace_attach(zygote_pid)) {
					printf("Error: ptrace zygote failed\n");
					return -1;
				}
				ptrace_setopt(zygote_pid, PTRACE_O_TRACEFORK | PTRACE_O_TRACECLONE | PTRACE_O_TRACESYSGOOD);
				continue;
			} else {
				return newpid;
			}
			/* ptrace(PTRACE_GETEVENTMSG, pid, NULL, &newpid); */
			/* printf("%d forks %d\n", pid, newpid); */
		}
		/* if zygote pauses of no reason, let it continue */
		ptrace(PTRACE_CONT, pid, NULL, NULL);
	}
	return -1;
}


