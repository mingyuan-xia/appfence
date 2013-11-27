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
#include "config.h"
#include "sandbox_helper.h"
#include "file_toolkit.h"
#include "ptraceaux.h"

#define IS_WRITE(oflag) ((oflag & O_WRONLY) != 0 || (oflag & O_RDWR) != 0)

//open syscall handler
pid_t syscall_open_handler(pid_t pid, int flag);
//default syscall handler
pid_t syscall_default_handler(pid_t pid, int flag);

pid_t ptrace_app_process(pid_t pid, int flag)
{
	/*
	if (ptrace_attach(pid)) {
		return -1;
	}
	*/
	printf("%d tracing process %d\n", getpid(), pid);

	// TODO why do we need to set syscall tracing too?
	// What if the app also fork an thread? even never meet this situation
	ptrace_setopt(pid, PTRACE_O_TRACEFORK | PTRACE_O_TRACEVFORK | PTRACE_O_TRACECLONE);


	while(pid > 0){
		//syscall enter
		long syscall_no =  ptrace_tool.ptrace_get_syscall_nr(pid);
		switch(syscall_no) {
			case __NR_open:
				pid = syscall_open_handler(pid, flag);
				break;
			default:
				pid = syscall_default_handler(pid, flag);
				break;
		}
	}
	ptrace_detach(pid);
	printf("%d exit\n", pid);
	return -1;
}

pid_t syscall_open_handler(pid_t pid, int flag) {
	int status;
	//arg1 is oflag
	long arg1 = ptrace_tool.ptrace_get_syscall_arg(pid, 1);
	long arg0 = ptrace_tool.ptrace_get_syscall_arg(pid, 0);

	int len = ptrace_strlen(pid, (void*) arg0);
	char path[len + 1];
	//first arg of open is path addr
	ptrace_tool.ptrace_read_data(pid, path, (void *)arg0, len + 1);
	if((flag & SANDBOX_FLAG) && check_prefix(path,SANDBOX_PATH)){
		char new_path[len + 1];
		//replace dir in path with LINK_PREFIX
		char* second_dir = get_nth_dir(path, 2);
		//check if need to replace upper level
		if(check_prefix(second_dir,SECOND_DIR))
			second_dir = get_nth_dir(path, 3);
		strcpy(new_path, SANDBOX_LINK);
		strcat(new_path, second_dir);
		ptrace_tool.ptrace_write_data(pid, new_path, (void*)arg0, len + 1);
		// create require folder
		create_path(new_path);
		printf("pid %d open: %s\n ==> new path: %s\n",pid,path, new_path);

		// return from open syscall, reset the path
		ptrace(PTRACE_SYSCALL, pid, NULL, NULL);
		pid = waitpid(pid, &status, __WALL);

		ptrace_tool.ptrace_write_data(pid, path, (void*)arg0, len + 1);

		ptrace(PTRACE_SYSCALL, pid, NULL, NULL);
		pid = waitpid(pid, &status, __WALL);
		return pid;
	} else {
		printf("pid %d open: %s\n",pid, path);
		return syscall_default_handler(pid, flag);
	}

}

pid_t syscall_default_handler(pid_t pid, int flag) {
	int status;
	ptrace(PTRACE_SYSCALL, pid, NULL, NULL);
	pid = waitpid(pid, &status, __WALL);
	//syscall return
	ptrace(PTRACE_SYSCALL, pid, NULL, NULL);
	pid = waitpid(pid, &status, __WALL);
	return pid;
}
