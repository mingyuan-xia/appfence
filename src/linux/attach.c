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

/*
int main(int argc, char *argv[])
{   
    pid_t traced_process;
    struct user_regs_struct regs;
    long ins;
    
    if(argc != 2) {
        printf("Usage: %s <pid to be traced>\n", argv[0], argv[1]);
        exit(1);
    }
    traced_process = atoi(argv[1]);
    ptrace(PTRACE_ATTACH, traced_process, NULL, NULL);
    wait(NULL);
    ptrace(PTRACE_GETREGS, traced_process, NULL, &regs);
    ins = ptrace(PTRACE_PEEKTEXT, traced_process, regs.eip, NULL);
    printf("EIP: %lx Instruction executed: %lx\n", regs.eip, ins);
    ptrace(PTRACE_DETACH, traced_process, NULL, NULL);
    return 0;
}
*/

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
}

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
* Try to find the entry and the exit of fork() in zygote
*
void __cyg_profile_func_enter (void *this_fn, void *call_site)
{
    int indent;

    if (stack_at == stack_size){
        unsigned size_new = stack_size ? stack_size * 2 : 0x100;
        struct stack *stack_new;

        stack_new = realloc (stack, sizeof (*stack) * size_new);
        if (!stack_new){
	         fprintf (stderr, PREFIX "stack size %u allocation failure!\n", size_new);
	         return;
	    }
        stack = stack_new;
        stack_size = size_new;
    }
    stack[stack_at].this_fn = this_fn;
    stack[stack_at].call_site = call_site;
    stack_at++;

    indent = stack_at - 1 <= INDENT_MAX ? stack_at - 1 : INDENT_MAX;

    fprintf (stderr, PREFIX "%*s->%s", indent, "", addr_print (this_fn));
    fprintf (stderr, " (%s)\n", addr_print (call_site));
}

void __cyg_profile_func_exit  (void *this_fn, void *call_site)
{
    int indent;

    indent = stack_at - 1 <= INDENT_MAX ? stack_at - 1 : INDENT_MAX;

    fprintf (stderr, PREFIX "%*s<-%s", indent, "", addr_print (this_fn));
    fprintf (stderr, " (%s)\n", addr_print (call_site));

    if (!stack_at){
        fprintf (stderr, PREFIX "stack empty!\n");
        return;
    }
    if (stack[stack_at - 1].this_fn != this_fn {
        || stack[stack_at - 1].call_site != call_site)
        fprintf (stderr, PREFIX "Unexpected function leave, resetting the stack\n");
        stack_at = 0;
        return;
    }
    stack_at--;
}*/

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

int main(int argc, char *argv[])
{   
    pid_t traced_process;
    struct user_regs_struct regs, newregs;
    long ins;
    /* int 0x80, int3 */
    char code[] = {0xcd,0x80,0xcc,0};
    char backup[4];
    if(argc != 2) {
        printf("Usage: %s <pid to be traced>\n", argv[0], argv[1]);
        exit(1);
    }
    traced_process = atoi(argv[1]);
    
    ptrace(PTRACE_ATTACH, traced_process, NULL, NULL);
    wait(NULL);
    ptrace(PTRACE_GETREGS, traced_process,NULL, &regs);
    
    /* Copy instructions into a backup variable */
    getdata(traced_process, regs.eip, backup, 3);
    
    /* Put the breakpoint */
    putdata(traced_process, regs.eip, code, 3);
    
    /* Let the process continue and execute the int 3 instruction */
    ptrace(PTRACE_CONT, traced_process, NULL, NULL);
    wait(NULL);
    
    printf("The process stopped, putting back the original instructions\n");
    printf("Press <enter> to continue\n");
    getchar();
    
    putdata(traced_process, regs.eip, backup, 3);
    /* Setting the eip back to the original instruction to let the process continue */
    ptrace(PTRACE_SETREGS, traced_process, NULL, &regs);
    ptrace(PTRACE_DETACH, traced_process, NULL, NULL);
    return 0;
}
