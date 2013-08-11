#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/user.h>
#include <sys/syscall.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h> 

const int long_size = sizeof(long);
const int ORIG_EAX = 11;


void getdata(pid_t child, long addr, char *str)
{   
	char *laddr;
	int i, j;
	union u {
		long val;
		char chars[long_size];
	}data;
	i = 0;
	j = 0;
	laddr = str;
	int done = 0;
	while(1) {
		data.val = ptrace(PTRACE_PEEKDATA, child, addr + i * 4, NULL);
		for (j=0; j<long_size; j++) {
			if (data.chars[j] == 0) {
				done = 1;
				break;
			}
		}
		if (done){
			memcpy(laddr, data.chars, j+1);
			break;
		}
		else {
			memcpy(laddr, data.chars, long_size);
			++i;
			laddr += long_size;
		}
	}
}

void putdata(pid_t child, long addr, char *str)
{   
	char *laddr;
	int i, j;
	union u {
		long val;
		char chars[long_size];
	}data;
	int len = strlen(str);
	i = 0;
	j = len/long_size;
	laddr = str;
	while(i < j) {
		memcpy(data.chars, laddr, long_size);
		ptrace(PTRACE_POKEDATA, child, addr + i * 4, data.val);
		++i;
		laddr += long_size;
	}
	j = len % long_size;
	if(j != 0) {
		memcpy(data.chars, laddr, j);
		data.chars[j] = 0;
		ptrace(PTRACE_POKEDATA, child, addr + i * 4, data.val);
	}
}

void createPath(char* dir)
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

char* changepath(char* str){
	char tmp[strlen(str)];
	strcpy(tmp, str);
	char* p = strtok(tmp, "/");
	if ((strcmp(p, "proc") == 0) || (strcmp(p, "run") == 0) || (strcmp(p, "lib") == 0) || (strcmp(p, "dev") == 0) || (strcmp(p, "etc") == 0) || (strcmp(p, "usr") == 0)) {
		return str;
	}
	char* homepath = getenv("HOME");
	char newPath[256];
	strcpy (newPath, homepath);
	strcat (newPath, "/sandbox");
	strcat(newPath, str);
	str = newPath;
	createPath(str);
	return str;
}

void trace(pid_t traced_process) {
	long orig_eax;
	int status;
	char *str;
	int toggle = 0;
	long params[2];
	while(1) {
		wait(&status);
		if(WIFEXITED(status)) {
			break;
		}
		orig_eax = ptrace(PTRACE_PEEKUSER, traced_process, 4 * ORIG_EAX, NULL);
		if(orig_eax == SYS_open) {
			if(toggle == 0) {
				toggle = 1;
				params[0] = ptrace(PTRACE_PEEKUSER, traced_process, 4 * 0, NULL);
				params[1] = ptrace(PTRACE_PEEKUSER, traced_process, 4 * 1, NULL);
				str = (char *)calloc(128, sizeof(char));
				getdata(traced_process, params[0], str);
				printf("filepath: %s\n", str);
				char* newPath = changepath(str);
				printf("new filepath: %s\n", newPath);
				putdata(traced_process, params[0], newPath);
			}
			else {
				toggle = 0;
			}
		}
		ptrace(PTRACE_SYSCALL, traced_process, NULL, NULL);
	}
}

int main(int argc, char *argv[])
{
	pid_t traced_process;
	if (strcmp(argv[1], "-p") == 0) {
		traced_process = atoi(argv[2]);	
		if(0 != ptrace(PTRACE_ATTACH, traced_process, NULL, NULL))
		{
			printf("Trace process failed.\n");
			return 1;
		}
		trace(traced_process);
	}
	else if (strcmp(argv[1], "-b") == 0) {
		traced_process = fork();
		if (traced_process == 0) {
			ptrace(PTRACE_TRACEME, 0, NULL, NULL);
			if (execlp(argv[2], NULL) == -1) {
				if (errno == ENOENT) {
					printf("filepath error\n");
				}
				else if (errno == EACCES) {
					printf("authority error\n");
				}
			}
		}
		else {
			trace(traced_process);
		}
	}
	
	return 0;
}
