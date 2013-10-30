/*
 * ptraceaux.h
 * Provide platform-neural access to the ptrace system call
 */

#ifndef PTRACEAUX_H_
#define PTRACEAUX_H_

#include <sys/types.h>

/**
 * Attach to a running process
 */
extern int ptrace_attach(pid_t pid);

/**
 * Detach a ptraced process
 */
extern int ptrace_attach(pid_t pid);

/**
 * Read data from a ptraced process
 */
extern void ptrace_read_data(pid_t pid, void *buf, void *addr, int nbytes);

/**
 * Write data to a ptraced process
 */
extern void ptrace_write_data(pid_t pid, void *buf, void *addr, int nbytes);

/**
 * Retrieve the system call number from a stopped ptraced process
 */
extern int ptrace_get_syscall_nr(pid_t pid);

/**
 * Set the option for a ptraced process
 */
extern void ptrace_setopt(pid_t pid, int opt);

#endif /* PTRACEAUX_H_ */
