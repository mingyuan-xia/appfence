/*
 * process_helper.h
 * The helper functions for communicating with the app process
 */

#ifndef PROCESS_HELPER_H_
#define PROCESS_HELPER_H_

#include <sys/types.h>
#include "util.h"

#define SANDBOX_FLAG 0x00000001

/**
 * Ptrace the app process until it forks a new process
 */
extern pid_t ptrace_app_process(pid_t pid, int flag);

void init_sandbox_state();

bool_t is_sandbox_enabled();

void set_sandbox_enabled(bool_t enabled);

#endif /* PROCESS_HELPER_H_ */

