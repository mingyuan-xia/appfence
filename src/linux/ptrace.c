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
			memcpy(laddr, data.chars, j);
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
	j = 0;
	laddr = str;
	while(i < len/long_size) {
		memcpy(data.chars, laddr, long_size);
		ptrace(PTRACE_POKEDATA, child, addr + i * 4, data.val);
		++i;
		laddr += long_size;
	}
	j = len % long_size;
	if(j != 0) {
		memcpy(data.chars, laddr, j);
		ptrace(PTRACE_POKEDATA, child, addr + i * 4, data.val);
	}
}

char* chpath(char* orin){

}

int main(int argc, char *argv[])
{
	pid_t traced_process;
	traced_process = atoi(argv[argc-1]);
	long orig_eax;
	long params[2];
	int status;
	char *str, *laddr;
	int toggle = 0;
	
	if(0 != ptrace(PTRACE_ATTACH, traced_process, NULL, NULL))
	{
		printf("Trace process failed.\n");
		return 1;
	}
	
	while(1) {
		wait(&status);
		if(WIFEXITED(status)) {
			break;
		}
		orig_eax = ptrace(PTRACE_PEEKUSER, traced_process, 4 * ORIG_EAX, NULL);
		if(orig_eax == SYS_open) {
			if(toggle == 0) {
				toggle = 1;
				printf("catch SYS_open\n");
				params[0] = ptrace(PTRACE_PEEKUSER, traced_process, 4 * 0, NULL);
				params[1] = ptrace(PTRACE_PEEKUSER, traced_process, 4 * 1, NULL);
				str = (char *)calloc(128, sizeof(char));
				getdata(traced_process, params[0], str);
				printf("filepath: %s\n", str);
			}
			else {
				toggle = 0;
				printf("exit SYS_open\n\n");
			}
		}
		ptrace(PTRACE_SYSCALL, traced_process, NULL, NULL);
	}
	return 0;
}
