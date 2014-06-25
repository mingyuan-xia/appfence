/*
 * util.h
 * The utility functions for aid.
 */

#ifndef UTIL_H_
#define UTIL_H_

typedef int bool_t;

#define true 1
#define false 0

#include <sys/types.h>

/*
 * if it acquire a class name, return non-zero.
 */
bool_t get_app_process_class_name(pid_t pid, char *class_name_buf,
		int class_name_buf_size);

pid_t find_app_process(const char *class_name);

void kill_app_process(bool_t force, const char *class_name);

void kill_all_process_in_whitelist(const char *list_path);

#endif /* UTIL_H_ */
