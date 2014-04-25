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
#include "binder_helper.h"

#define IS_WRITE(oflag) ((oflag & O_WRONLY) != 0 || (oflag & O_RDWR) != 0)
#define DEV_BINDER "/dev/binder"

//open syscall handler
pid_t syscall_open_handler(pid_t pid, int *binder_pid, int flag);

//ioctl syscall handler
pid_t syscall_ioctl_handler(pid_t pid, pid_t binder_pid, int flag);

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

	int binder_fd = -1;
	int status;
	

	ptrace(PTRACE_SYSCALL, pid, NULL, NULL);
	pid = waitpid(-1, &status, __WALL);
	while(pid > 0){
		if(IS_FORK_EVENT(status) || IS_CLONE_EVENT(status)){
			pid_t newpid;
			ptrace(PTRACE_GETEVENTMSG, pid, NULL, (long) &newpid);
			/* ptrace(PTRACE_SYSCALL, newpid, NULL, NULL); */
			printf("new thread %d .\n", newpid);
			ptrace(PTRACE_SYSCALL, pid, NULL, NULL);
		} else{
			//syscall enter
			long syscall_no =  ptrace_tool.ptrace_get_syscall_nr(pid);
			printf("syscall from %d\n", pid);
			switch(syscall_no) {
				case __NR_open:
					pid = syscall_open_handler(pid, &binder_fd, flag);
					break;
				/* case __NR_ioctl: */
				/* 	pid = syscall_ioctl_handler(pid, binder_fd, flag); */
				/* 	break; */
				default:
					pid = syscall_default_handler(pid, flag);
					break;
			}

		}
		pid = waitpid(-1, &status, __WALL);
	}
	ptrace_detach(pid);
	printf("%d exit\n", pid);
	return -1;
}

pid_t syscall_open_handler(pid_t pid, int *binder_fd, int flag)
{
	int status;
	//arg1 is oflag
	long arg1 = ptrace_tool.ptrace_get_syscall_arg(pid, 1);
	long arg0 = ptrace_tool.ptrace_get_syscall_arg(pid, 0);
	printf("lala\n");

	int len = ptrace_strlen(pid, (void*) arg0);
	printf("lala\n");
	char path[len + 1];
	//first arg of open is path addr
	ptrace_tool.ptrace_read_data(pid, path, (void *)arg0, len + 1);

	/* if((flag & SANDBOX_FLAG) && check_prefix(path,SANDBOX_PATH)){ */
	/* 	char new_path[len + 1]; */
	/* 	//replace dir in path with LINK_PREFIX */
	/* 	char* second_dir = get_nth_dir(path, 2); */
	/* 	//check if need to replace upper level */
	/* 	if(check_prefix(second_dir,SECOND_DIR)) */
	/* 		second_dir = get_nth_dir(path, 3); */
	/* 	strcpy(new_path, SANDBOX_LINK); */
	/* 	strcat(new_path, second_dir); */
	/* 	ptrace_tool.ptrace_write_data(pid, new_path, (void*)arg0, len + 1); */
	/* 	// create require folder */
	/* 	create_path(new_path); */
	/* 	printf("pid %d open: %s\n ==> new path: %s\n",pid,path, new_path); */

	/* 	// return from open syscall, reset the path */
	/* 	ptrace(PTRACE_SYSCALL, pid, NULL, NULL); */
	/* 	pid = waitpid(pid, &status, __WALL); */

	/* 	ptrace_tool.ptrace_write_data(pid, path, (void*)arg0, len + 1); */

	/* 	ptrace(PTRACE_SYSCALL, pid, NULL, NULL); */
	/* 	/1* pid = waitpid(-1, &status, __WALL); *1/ */
	/* 	return pid; */
	/* } else if(strcmp(path, DEV_BINDER) == 0) { */
	/* 	// return from open syscall, read the result fd */
	/* 	ptrace(PTRACE_SYSCALL, pid, NULL, NULL); */
	/* 	pid = waitpid(pid, &status, __WALL); */

	/* 	// reg0 will be the result of open */
	/* 	*binder_fd = (int) ptrace_tool.ptrace_get_syscall_arg(pid , 0); */
	/* 	printf("pid %d open binder: %d\n", pid, *binder_fd); */

	/* 	ptrace(PTRACE_SYSCALL, pid, NULL, NULL); */
	/* 	/1* pid = waitpid(-1, &status, __WALL); *1/ */
	/* 	return pid; */
		
	/* } else { */
		printf("pid %d open: %s\n",pid, path);
		return syscall_default_handler(pid, flag);
	/* } */

}

pid_t syscall_ioctl_handler(pid_t pid, int binder_fd, int flag)
{
	if(binder_fd < 0) {
		return syscall_default_handler(pid, flag);
	}

	int status;
	// arg0 will be the file description
	long arg0 = ptrace_tool.ptrace_get_syscall_arg(pid, 0);

	if((int)arg0 == binder_fd) {
		binder_ioctl_handler(pid);
	}

	return syscall_default_handler(pid, flag);
}

pid_t syscall_default_handler(pid_t pid, int flag)
{
	int status;
	ptrace(PTRACE_SYSCALL, pid, NULL, NULL);
	pid = waitpid(pid, &status, __WALL);
	//syscall return
	ptrace(PTRACE_SYSCALL, pid, NULL, NULL);
	/* pid = waitpid(-1, &status, __WALL); */
	return pid;
}
