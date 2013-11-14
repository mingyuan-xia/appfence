/*
 * process_helper.c
 * Implementation of process_helper.h
 */
#include <sys/ptrace.h>
#include <sys/syscall.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <signal.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "sandbox_helper.h"
#include "ptraceaux.h"

#define SANDBOX_PATH "/data/data /sdcard /mnt/sdcard /storage/sdcard"

#define SANDBOX_PREFIX "/data/sandbox"

// check if path is begin with DATA_DATA SDCARD or MNT_SDCARD
int is_data_path(char* path);
// create folder base on dir
void createPath(char* dir);

#define IS_WRITE(oflag) ((oflag & O_WRONLY) != 0 || (oflag & O_RDWR) != 0)

pid_t ptrace_app_process(pid_t pid, int sandbox)
{
	if (ptrace_attach(pid)) {
		return -1;
	}
	printf("%d tracing process %d\n", getpid(), pid);

	ptrace_setopt(pid, PTRACE_O_TRACEFORK | PTRACE_O_TRACEVFORK | PTRACE_O_TRACECLONE);

	int status;

	while(pid > 0){
		//syscall enter
		long syscall_no =  ptrace_tool.ptrace_get_syscall_nr(pid);
		if (syscall_no == __NR_open) {
			//arg1 is oflag
			long arg1 = ptrace_tool.ptrace_get_syscall_arg(pid, 1);
			//TODO: determine which file need to keep isolation
			long arg0 = ptrace_tool.ptrace_get_syscall_arg(pid, 0);
			
			char* sandbox_prefix = SANDBOX_PREFIX;
			int prefix_len = strlen(sandbox_prefix);

			int len = ptrace_strlen(pid, (void*) arg0);
			char path[len + 1 + prefix_len];
			//first arg of open is path addr
			ptrace_tool.ptrace_read_data(pid, path, (void *)arg0, len + 1 + prefix_len);
			printf("pid %d open: %s\n",pid, path);
			if(sandbox && is_data_path(path)){
				// change to new path
				char new_path[len + 1 + prefix_len];
				strcpy(new_path, sandbox_prefix);
				strcat(new_path, path);
				ptrace_tool.ptrace_write_data(pid, new_path, (void*)arg0, len + 1 + prefix_len);
				// create require folder
				createPath(new_path);
				printf("pid %d new path: %s\n",pid, new_path);

				// return from open syscall, reset the path
				ptrace(PTRACE_SYSCALL, pid, NULL, NULL);
				pid = waitpid(pid, &status, __WALL);

				ptrace_tool.ptrace_write_data(pid, path, (void*)arg0, len + 1 + prefix_len);

				ptrace(PTRACE_SYSCALL, pid, NULL, NULL);
				pid = waitpid(pid, &status, __WALL);
				continue;
			}
		}
		ptrace(PTRACE_SYSCALL, pid, NULL, NULL);
		pid = waitpid(pid, &status, __WALL);
		//syscall return
		ptrace(PTRACE_SYSCALL, pid, NULL, NULL);
		pid = waitpid(pid, &status, __WALL);
	}
	ptrace_detach(pid);
	printf("%d exit\n", pid);
	return -1;
}

int is_data_path(char* path)
{
	char* sandbox_path = SANDBOX_PATH;
	char* c = sandbox_path;
	int i = 0;
	int result = 1;
	for(; *c != 0; c++,i++){
		if (result && *c == ' ') {
			return 1;
		}
		else if (*c == ' ' ) {
			i = -1;
			result = 1;
		} else if (*c != path[i]) {
			result = 0;
		}
	}
	return result;
}

void createPath(char* dir)
{
	int	i,len;
	char str[512];
	strcpy(str, dir);
	len = strlen(str);
	for (i=0; i<len; i++)
	{
		if (str[i] == '/')
		{
			str[i] = '\0';
			if (access(str, F_OK) != 0)
			{
				mkdir(str, 0777);
			}
			str[i] = '/';
		}
	}
	return;
}
