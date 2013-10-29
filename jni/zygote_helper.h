/*
 * zygote_helper.h
 * The helper functions for communicating with the zygote process
 */

#ifndef ZYGOTE_HELPER_H_
#define ZYGOTE_HELPER_H_

#include <sys/types.h>

extern pid_t zygote_find_process(void);
extern void ptrace_zygote(pid_t pid);

#endif /* ZYGOTE_HELPER_H_ */
