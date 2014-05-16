/*
 * file_toolkit.c
 * implementation of file_toolkit.h
 */

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include "file_toolkit.h"


int check_prefix(char* path, char* prefix_list)
{
	char* c = prefix_list;
	int i = 0;
	int result = 1;
	for(; *c != 0; c++,i++){
		if (result && *c == ' ') {
			return 1;
		} else if (*c == ' ' ) {
			i = -1;
			result = 1;
		} else if (*c != path[i]) {
			i = -1;
			result = 0;
		}
	}
	return result;
}

int check_prefix_dir(char* path, char* prefix_list)
{
	char* c = prefix_list;
	int i = 0;
	int result = 1;
	int num = 0;
	for(; *c != 0; c++,i++){
		if (result && *c == ' ') {
			return num;
		} else if (*c == ' ' ) {
			i = -1;
			result = 1;
			num = 0;
		} else if (*c != path[i]) {
			i = -1;
			result = 0;
			num = 0;
		} else if (result && path[i] == '/') {
			num++;
		}
	}
	return num;
}

char* get_nth_dir(char* path, int n)
{
	if(n <= 0)
		return 0;

	int num = 0;
	char* result = path;
	while(*result != 0){
		if(*result == '/'){
			num++;
			if(num == n)
				return result;
		}
		result++;
	}
	return result;
}

int create_nth_dir(char* dir, int n, uid_t owner, gid_t group, mode_t mod)
{
	if(n < 1)
		return -1;
	int i,len;
	int result = -1;
	len = strlen(dir);
	char str[len + 1];
	strcpy(str, dir);
	int level = -1;
	for (i=0; i<len; i++){
		if(str[i] == '/'){
			level++;
			if(level == n){
				str[i] = '\0';
				if ((result = access(str, F_OK)) != 0){
					result = mkdir(str, mod);
				}
				if(result == 0){
					result = chown(str, owner, group);
				}
				return result;
			}
		}
	}
	return result;
}

int create_link(char* sandbox_path, char* link_path) {
	/* create_path(sandbox_path); */
	/* create_path(link_path); */
	return symlink(sandbox_path, link_path);
}
