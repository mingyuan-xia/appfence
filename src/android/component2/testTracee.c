/*
* This program is created temporarily for testing attrace.c
*/

/*
* testTracee
*/
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

int main(){
    pid_t child=fork();
    if(child){
        printf("create a child %d" , child);
        sleep(1);
    }
    else{
        printf("I'm a child");
        sleep(1);
    }
}
