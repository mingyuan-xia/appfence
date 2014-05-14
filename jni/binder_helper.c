/*
 * binder_helper.c
 * implementation of binder_helper.h
 */

#include <stdio.h>
#include <linux/binder.h>
#include <string.h>
#include "uchar.h"
#include "binder_helper.h"
#include "ptraceaux.h"
#include "config.h"

void binder_write_read_handler(pid_t pid);

void binder_ioctl_handler(pid_t pid)
{
	// arg1 is cmd in binder ioctl
	long arg1 = ptrace_get_syscall_arg(pid, 1);

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
	tracee_ptr_t arg2 = ptrace_get_syscall_arg(pid, 2);
	
	ptrace_read_data(pid, &wr_buffer, arg2, sizeof(wr_buffer));

	
	// have write command
	if(wr_buffer.write_size > 0) {
		int i = 0;
		uint32_t cmd;
		tracee_ptr_t cur = (tracee_ptr_t) (wr_buffer.write_buffer + wr_buffer.write_consumed);
		while(cur < (unsigned long) (wr_buffer.write_size + wr_buffer.write_buffer)) {
			/* printf("%d:  ",i++); */
			//read binde command from write buffer
			ptrace_read_data(pid, &cmd, cur, sizeof(uint32_t));
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
						ptrace_read_data(pid, &data, cur, sizeof(struct binder_transaction_data));
						cur += sizeof(struct binder_transaction_data);
						/* if(data.code == GET_SERVICE_TRANSACTION){ */
						int i;

						tracee_ptr_t ptr = (tracee_ptr_t)data.data.ptr.buffer + 4;

						// the request to service manager
						long len;

						// include/binder/parcel.cpp
						ptrace_read_data(pid, &len, ptr, sizeof(long));
						char16_t service[len + 1];
						ptrace_read_data(pid, service, ptr + sizeof(long), sizeof(char16_t) * (len + 1));
						ptr +=(sizeof(long) + 2 + (len + 1) * sizeof(char16_t));
						/* printf("service name: %ld ---", len); */
						/* for(i = 0; i < len; i++) { */
						/* 	printf("%c",(char)service[i]); */
						/* } */
						/* printf("\n"); */

						// TODO: identify service base on service name and handler different service separately

						if(CONTENT_SANDBOX_ENABLED && strcmp12(service, ICONTENT_PROVIDER) == 0 || (strcmp12(service, IACTIVITY_MANAGER) == 0)){ //&& data.code == GET_CONTENT_PROVIDER_TRANSACTION)){
							char16_t all_data[data.data_size / 2];
							ptrace_read_data(pid, (void *)all_data, (tracee_ptr_t)data.data.ptr.buffer, data.data_size);
							int com = 0;
							for(i = 0; i < data.data_size / 2; i++){
								if(strpreg12(&all_data[i], SANDBOX_CONTENT_PROVIDER) == 0) {
									com = i;
									strchpre12(&all_data[i], SANDBOX_CONTENT_PROVIDER_FAKE_PREFIX);
								}
								printf("%c", all_data[i]);
							}
							printf("\n");
							if(com > 0){
								/* for(i = 0; i < data.data_size / 2; i++){ */
								/* 	printf("%c|", all_data[i]); */
								/* } */
								/* printf("\n"); */
								ptrace_write_data(pid, all_data, (tracee_ptr_t)  data.data.ptr.buffer, data.data_size);
							}
						}
							/* printf("service name: "); */
							/* ptrace_read_data(pid, &len, (void *)ptr, sizeof(int)); */
							/* printf("%d ---- ", len); */
							/* char16_t service_name[len + 1]; */
							
						/* } */
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
