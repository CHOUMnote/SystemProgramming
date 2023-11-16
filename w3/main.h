#ifndef __PRE_H
#define __PRE_H
#include "main_macro.h"
#endif

#ifndef __MAIN_H
#define __MAIN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>

int fd;
caddr_t addr;
struct stat statbuf;
pid_t pid;
int s_to_c[2]; // server -> client pipe
int c_to_s[2]; // client -> server pipe

struct STUDENT {
    int s_id;
    char s_name[32];
    int s_age;
    char s_addr[32];
    int s_grade;
};

inline void init() {
    //open pipe
    if (pipe(s_to_c) || pipe(c_to_s)){
        perror("pipe");
        exit(1);
    }

    //open file
    fd = open(DATA, O_RDWR | O_CREAT | O_APPEND, 0666);
    if (fd == -1) {
        perror("file open");
        exit(1);
    }
    
    if(ftruncate(fd, BUFSIZ) == -1){
        perror("file resize");
        exit(1);
    }

    if(stat(DATA, &statbuf) == -1){
        perror("stat");
        exit(1);
    }

    //memory mapping
    addr = mmap(NULL, statbuf.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (addr == MAP_FAILED) {
        perror("stat");
        exit(1);
    }

    close(fd); 
}
inline void esc(){
    close(s_to_c[0]);
    close(s_to_c[1]);
    close(c_to_s[0]);
    close(c_to_s[1]);

    munmap(addr, BUFSIZ);
}

#endif