#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>

int main(int a, char* argv[]){
    int errno;
    char *msg;
    if(access("test.c",F_OK) == -1){
        msg = strerror(errno); 
        perror("access");
    }
}