/*
 * ptraceaux.h
 * Provide platform-neural access to the ptrace system call
 */

#ifndef PTRACEAUX_H_
#define PTRACEAUX_H_

#include <sys/types.h>

/* from the man page of ptrace */
#define IS_FORK_EVENT(status) (status>>8 == (SIGTRAP | (PTRACE_EVENT_FORK<<8)))
#define IS_VFORK_EVENT(status) (status>>8 == (SIGTRAP | (PTRACE_EVENT_VFORK<<8)))
#define IS_CLONE_EVENT(status) (status>>8 == (SIGTRAP | (PTRACE_EVENT_CLONE<<8)))
#define IS_SYSCALL_EVENT(status) (WIFSTOPPED(status) && WSTOPSIG(status) & 0x80)
/* #define IS_FORK_EVENT(status) (status>>16 == PTRACE_EVENT_FORK) */
/* #define IS_CLONE_EVENT(status) (status>>16 == PTRACE_EVENT_CLONE) */

/* TODO: imagine a 64-bit tracer tracing a 32-bit tracee */
typedef void *tracee_ptr_t;
/* Linux boxes follow LP64, i.e., long and pointers are 64-bit, int is 32-bit 
 * Windows boxes follow LLP64, i.e. long long and pointers are 64-bit, int and long are 32-bit
 * */
typedef unsigned long tracer_word_t;

#define TRACER_WORD_SIZE (sizeof(tracer_word_t))

/**
 * Attach to a running process
 */
extern int ptrace_attach(pid_t pid);

/**
 * Detach a ptraced process
 */
extern int ptrace_detach(pid_t pid);

/**
 * Set the option for a ptraced process
 */
extern void ptrace_setopt(pid_t pid, int opt);

/**
 * Continue the execution of tracee
 */
extern void ptrace_cont(pid_t pid);

/* Architecture dependent */
extern void ptrace_read_data (pid_t pid, void *buf, tracee_ptr_t addr, int nbytes);
extern int ptrace_strlen (pid_t pid, tracee_ptr_t addr);
extern void ptrace_write_data (pid_t pid, void *buf, tracee_ptr_t addr, int nbytes);
extern long ptrace_get_syscall_nr (pid_t pid);
extern long ptrace_get_syscall_arg (pid_t pid, int n);

#endif /* PTRACEAUX_H_ */

