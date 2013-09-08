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

#include <stdio.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/user.h>
#include <sys/syscall.h>   /* For SYS_fork etc */
#include <string.h>
#include <linux/ptrace.h>
#include <asm-i486/ptrace.h> /* For constants ORIG_EAX etc */

int main(int argc , char *argv[])
{
    pid_t child;

    if(argc != 2){
        fprintf(stderr , "usage: %s program" , argv[0]);
        return 1;
    }

    child = fork();
    if(child == 0) {
        sleep(1);
        execl(argv[1] , argv[1] , NULL);
    }
    else {
        int status;
        long data = PTRACE_O_TRACEEXIT |PTRACE_O_TRACEFORK ;

        ptrace(PTRACE_ATTACH , child , NULL , NULL);
        ptrace(PTRACE_SETOPTIONS , child , NULL , data);
        while(1){
            if(wait(&status) == -1){
                perror("wait");
                return 0;
            }
            printf("the child process stops. status: %d, signal? %d, exit? %d, continue? %d, stop? %d" ,
                   WEXITSTATUS(status) , WIFSIGNALED(status) , WIFEXITED(status) , WIFCONTINUED(status) , WIFSTOPPED(status));
            if(WSTOPSIG(status) == SIGTRAP){
                if(status & (PTRACE_EVENT_EXIT << 8)){
                    printf("get the child process while it's exiting");
                    break;
                }
                else if(status & (PTRACE_EVENT_FORK << 8)){
                    printf("the child process fork a new process");
                }
            }
            if (ptrace(PTRACE_CONT, child, 0, /*WSTOPSIG(status)*/SIGSTOP)) {
                perror("stopper: ptrace(PTRACE_CONT, ...)");
                return 1;      
            }
        } 
        if (ptrace(PTRACE_CONT, child, 0, 0)) {
            perror("stopper: ptrace(PTRACE_CONT, ...)");
            return 1;
        }
    }
    return 0;
}

/*
int main(int argc, char * argv[])
{   
    pid_t child;
    long orig_eax, eax;
    long params[3];
    int status;
    int insyscall = 0;
 
    if(argc < 2){
        printf("need pid\n");
        return 0;
    }


    child = atoi(argv[1]);
    printf("try to attch pid [%d]\n", child);
    ptrace(PTRACE_ATTACH, child, 0, 0);

    int count = 20;
    while(count-- > 0) {
        wait(&status);
        printf("recv sig [%d]\n", WSTOPSIG(status));
        if(WIFEXITED(status)){
             printf("The child has been dead\n");
             break;
        }
        struct user_regs_struct regs;
        ptrace(PTRACE_GETREGS, child, NULL, &regs);
        printf("EIP: %lx , orig_eax %lx, eax %lx\n", regs.eip, regs.orig_eax, regs.eax);

        printf("PTRACE_SYSCALL\n");
        ptrace(PTRACE_SYSCALL, child, NULL, NULL);

        printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n");
    }

    return 0;
}*/

/*
const int long_size = sizeof(long);

void getdata(pid_t child, long addr, char *str, int len)
{   
    char *laddr;
    int i, j;
    
    union u {
        long val;
        char chars[long_size];
    }data;
    
    i = 0;
    j = len / long_size;
    laddr = str;
    
    while(i < j) {
        data.val = ptrace(PTRACE_PEEKDATA, child, addr + i * 4, NULL);
        memcpy(laddr, data.chars, long_size);
        ++i;
        laddr += long_size;
    }
    
    j = len % long_size;
    if(j != 0) {
        data.val = ptrace(PTRACE_PEEKDATA, child, addr + i * 4, NULL);
        memcpy(laddr, data.chars, j);
    }
    str[len] = '\0';
}*/

/*
void tracePro(int pid)
{
    int len = 4;
    char insertcode[] = " ";
        
    putdata(pid,  , insertcode, len);
}
    
int main(int argc, char *argv[])
{   
    if(argc != 2) {
        printf("Usage: %s <pid to be traced>\n", argv[0], argv[1]);
        return 1;
    }
    pid_t traced_process;
    int status;
    traced_process = atoi(argv[1]);
    
    if(0 != ptrace(PTRACE_ATTACH, traced_process, NULL, NULL)){
        printf("Trace process failed:%d.\n", errno);
        return 1;
    }
    tracePro(traced_process);
    ptrace(PTRACE_DETACH, traced_process, NULL, NULL);
    return 0;
}*/


/*
void putdata(pid_t child, long addr, char *str, int len)
{   
    char *laddr;
    int i, j;
    
    union u {
        long val;
        char chars[long_size];
    }data;
    
    i = 0;
    j = len / long_size;
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
        ptrace(PTRACE_POKEDATA, child, addr + i * 4, data.val);
    }
}


