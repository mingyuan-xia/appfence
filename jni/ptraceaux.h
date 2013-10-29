/*
 * ptraceaux.h
 * Provide platform-neural access to the ptrace system call
 */

#ifndef PTRACEAUX_H_
#define PTRACEAUX_H_

extern int ptrace_attach(pid_t pid);
extern int ptrace_read_data(pid_t pid, void *buf, void *addr, int nbytes);
extern int ptrace_write_data(pid_t pid, void *buf, void *addr, int nbytes);
extern int ptrace_get_syscall_nr(pid_t pid);

#endif /* PTRACEAUX_H_ */
