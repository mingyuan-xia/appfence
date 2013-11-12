/*
 * ptraceaux.h
 * Provide platform-neural access to the ptrace system call
 */

#ifndef PTRACEAUX_H_
#define PTRACEAUX_H_

#include <sys/types.h>

#define ARCH_X86 1
#define ARCH_ARM 2

/**
 * Attach to a running process
 */
extern int ptrace_attach(pid_t pid);

/**
 * Detach a ptraced process
 */
extern int ptrace_detach(pid_t pid);

struct {
	void (*ptrace_read_data) (pid_t pid, void *buf, void *addr, int nbytes);
	int (*ptrace_strlen) (pid_t pid, void *addr);
	void (*ptrace_write_data) (pid_t pid, void *buf, void *addr, int nbytes);
	int (*ptrace_get_syscall_nr) (pid_t pid);

} ptrace_tool;

void init_ptrace_tool(int arch);


/**
 * Set the option for a ptraced process
 */
extern void ptrace_setopt(pid_t pid, int opt);

/**
 *  Handle STOPSIG from child process forked by zygote
 */
extern int handle_syscall(pid_t pid);


#endif /* PTRACEAUX_H_ */
