/*
 * binder_helper.c
 * implementation of binder_helper.h
 */

#include <stdio.h>
#include <linux/binder.h>
#include "binder_helper.h"
#include "ptraceaux.h"

void binder_write_read_handler(pid_t pid);

void binder_ioctl_handler(pid_t pid)
{
	// arg1 is cmd in binder ioctl
	long arg1 = ptrace_tool.ptrace_get_syscall_arg(pid, 1);

	switch(arg1) {
		case BINDER_WRITE_READ:
			binder_write_read_handler(pid);
			break;
		default:
			break;
	}
}

void binder_write_read_handler(pid_t pid)
{
	struct binder_write_read wr_buffer;
	// arg2 is pointer to the buffer of ioctl
	unsigned long arg2 = ptrace_tool.ptrace_get_syscall_arg(pid, 2);
	
	ptrace_tool.ptrace_read_data(pid, &wr_buffer, (void *)arg2, sizeof(wr_buffer));

	
	// have write command
	if(wr_buffer.write_size > 0) {
		int i = 0;
		uint32_t cmd;
		unsigned long cur = (unsigned long) (wr_buffer.write_buffer + wr_buffer.write_consumed);
		while(cur < (unsigned long) (wr_buffer.write_size + wr_buffer.write_buffer)) {
			/* printf("%d:  ",i++); */
			//read binde command from write buffer
			ptrace_tool.ptrace_read_data(pid, &cmd, (void *)cur, sizeof(uint32_t));
			cur += sizeof(uint32_t);

			
			//depatch differen command
			switch(cmd){
				case BC_INCREFS:
				case BC_ACQUIRE:
				case BC_RELEASE:
				case BC_DECREFS:
				case BC_FREE_BUFFER:
				case BC_DEAD_BINDER_DONE:
					cur += sizeof(uint32_t);
					break;
				case BC_INCREFS_DONE:
				case BC_ACQUIRE_DONE:
				case BC_REQUEST_DEATH_NOTIFICATION:
				case BC_CLEAR_DEATH_NOTIFICATION:
					cur += 2 * sizeof(uint32_t);
					break;
				case BC_REPLY:
					cur += sizeof(struct binder_transaction_data);
					break;
				case BC_TRANSACTION: {
					//only handle transaction
						struct binder_transaction_data data;
						ptrace_tool.ptrace_read_data(pid, &data, (void *)cur, sizeof(struct binder_transaction_data));
						cur += sizeof(struct binder_transaction_data);
						if(data.target.handle == 0x0){
							// the request to service manager
							printf("get transaction to service manager\n");
						}
					}
					break;
				default:
					printf("error: process: %d get unknow command: %d\n", pid, cmd);
					break;
			}
			/* printf("\n"); */
		}
	}
}
