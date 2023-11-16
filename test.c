#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>

pid_t pid;
int fd[2];

void main(){
    if(pipe(fd)){
        perror("pipe error");
        return;
    }

    if((pid = fork())<0){
        perror("fork");
        exit(1);
    }

    if(pid ==0){
        close(fd[1]);
        char msg[BUFSIZ];
        while(1){
            read(fd[0], msg, BUFSIZ);
            printf("%s\n", msg);
        }
        return;
    }else{
        close(fd[0]);
        char input_buf[BUFSIZ];
        while(1){
            scanf("%s",input_buf);
            if(!strcmp(input_buf, "exit")){
                kill(pid, SIGINT);
                break;
            }else{
                write(fd[1], input_buf, BUFSIZ);
            }
        }
        wait(NULL);
        return;
    }
}