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
		}
		else if (*c == ' ' ) {
			i = -1;
			result = 1;
		} else if (*c != path[i]) {
			result = 0;
		}
	}
	return result;
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
	return 0;
}

void create_path(char* dir)
{
	int	i,len;
	char str[512];
	strcpy(str, dir);
	len = strlen(str);
	for (i=0; i<len; i++)
	{
		if (str[i] == '/')
		{
			str[i] = '\0';
			if (access(str, F_OK) != 0)
			{
				mkdir(str, 0777);
			}
			str[i] = '/';
		}
	}
	return;
}

int create_link(char* sandbox_path, char* link_path) {
	create_path(sandbox_path);
	create_path(link_path);
	return symlink(sandbox_path, link_path);
}
