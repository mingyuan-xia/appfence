#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h> 

extern void trace(pid_t traced_process);

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
	
	return 0;
}
