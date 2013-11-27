/*
 * file_toolkit.h
 * This header contains several function need to use to handle file issue
 */

#ifndef FILE_TOOLKIT_H_
#define FILE_TOOLKIT_H_

// check if path is begin with string in prefix_list(seperated by space)
int check_prefix(char* path, char* prefix_list);

// get the nth '/' in path
char* get_nth_dir(char* path, int n);

// create folder base on dir
void create_path(char* dir);

// create link of sandbox
int create_link(char* sandbox_path, char* link_path);


#endif
