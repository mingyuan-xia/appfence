/*
* This program is modified for android to call fork() in our appfence process 
* to create a pprocess,which enables ptrace to do the later work including 
* attaching to zygote,intercepting fork() in zygote,and even returning the
* process we forked in appfence to release zygote.
*
* Here is just a version for linux ptrace of using ptrace_attach to set up 
* checkpoints and supplant some instructions.
*
* The later version with full functions mentioned above will be work out.
*/

/*
* attach
*/

#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <linux/user.h>
#include <sys/syscall.h>
 
const int long_size = sizeof(long);

/*
void modifyPidForkedByZygote()
{
}
 
void setuidForPtraceChild(char *str)
{ 
}*/

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

int main()
{ 
    pid_t child;
    child = fork();
    if(child == 0) {
       ptrace(PTRACE_TRACEME, 0, NULL, NULL);
       execl("/bin/ls", "ls", NULL);
    }
    else {
        long orig_eax, returnValue;
        long params[1];
        int status;
        char *str, *laddr;
        int toggle = 0;
        while(1) {
            wait(&status);
            if(WIFEXITED(status))
              break;
            orig_eax = ptrace(PTRACE_PEEKUSER, child, 4 * ORIG_EAX, NULL);
            if(orig_eax == SYS_fork) {
                if(insyscall == 0) 
                    insyscall = 1;
                else {
                    returnValue=ptrace(PTRACE_PEEKUSER, pid, EAX*4, NULL);
                    printf("process forked by zygote with return value= %ld\n", returnValue);
                    insyscall = 0;
                }
            }
            ptrace(PTRACE_SYSCALL, child, NULL, NULL);
        } 
    }
    return 0;
}
