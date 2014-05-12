/*
 * process_helper.h
 * The helper functions for communicating with the app process
 */

#ifndef PROCESS_HELPER_H_
#define PROCESS_HELPER_H_

#include <sys/types.h>

#define SANDBOX_FLAG 0x00000001

typedef void *tracee_ptr_t;

 /**
  * Ptrace the app process until it forks a new process
  */
 extern pid_t ptrace_app_process(pid_t pid, int flag);

#endif /* PROCESS_HELPER_H_ */

