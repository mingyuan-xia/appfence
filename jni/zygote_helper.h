/*
 * zygote_helper.h
 * The helper functions for communicating with the zygote process
 */

#ifndef ZYGOTE_HELPER_H_
#define ZYGOTE_HELPER_H_

#include <sys/types.h>

/**
 * Find the zygote process pid from the OS
 */
extern pid_t zygote_find_process(void);

/**
 * Ptrace the zygote process until it forks a new process
 */
extern pid_t ptrace_zygote(pid_t zygote_pid);

#endif /* ZYGOTE_HELPER_H_ */
