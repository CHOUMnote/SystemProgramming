#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h> 
#include <sys/stat.h>

#include "dir.h"

int S_MOD = 0;
int H_MOD = 0;
int R_MOD = 0;
int L_MOD = 0;
char* SORT_BY = "";

void print_header(){
    char *msg = "myls 사용 설명서\n-h\t:\t현재 프로그램의 명령어를 출력합니다.\n-l\t:\t파일 및 디렉토리에 대한 디테일한 정보를 출력합니다.\n";
    printf("%s", msg);
    msg = "-r\t:\t서브 디렉토리 내용도 함께 출력합니다.\n-s\t:\tname,size,type,time 매개변수로 정렬해서 출력합니다.\n";
    printf("%s", msg);
}

void print_list(){
    DIR *dp = opendir(".");
    struct dirent* dent;
    char *cwd = getcwd(NULL, BUFSIZ);

    printf("현재 경로 : %s\n", cwd);
    printf("현재 경로 모든 파일 및 디렉토리 출력\n");
    while ((dent = readdir(dp))){
        printf("%s\n", dent->d_name);
    }

    free(cwd);
    closedir(dp);
}

void print_error(){
    printf("에러!\n");
}

void print_content(struct Content* a){
    char* t;
    switch(a->type){
        case DIRR:
            t = "DIR";
            break;
        case LINK:
            t = "LNK";
            break;
        case C_DEV:
            t = "CHR";
            break;;
        case B_DEV:
            t = "BLK";
            break;
        case NORMAL:
            t = "REG";
            break;
        case MISSING:
            exit(1);
            break;
    }
    printf("%-15s %-15ld %-15ld %-15s\n",a->name, a->size, a->time, t);
}

int compareByName(const void *a, const void *b) {
    const struct Content *contentA = (const struct Content *)a;
    const struct Content *contentB = (const struct Content *)b;
    return strcmp(contentA->name, contentB->name);
}

int compareBySize(const void *a, const void *b) {
    const struct Content *contentA = (const struct Content *)a;
    const struct Content *contentB = (const struct Content *)b;
    return contentA->size - contentB->size;
}

int compareByType(const void *a, const void *b) {
    const struct Content *contentA = (const struct Content *)a;
    const struct Content *contentB = (const struct Content *)b;
    return contentA->type - contentB->type;
}

int compareByTime(const void *a, const void *b) {
    const struct Content *contentA = (const struct Content *)a;
    const struct Content *contentB = (const struct Content *)b;
    return contentA->time - contentB->time;
}

void print_sort_list(char* a){
    int size = 0;
    char *cwd = getcwd(NULL, BUFSIZ);
    DIR *dp = opendir(".");
    struct dirent* dent;
    struct stat statbuf;

    while((dent=readdir(dp))){
        size++;
    }
    rewinddir(dp);

    struct Content *contents = (struct Content *)malloc(size * sizeof(struct Content));
    if(contents==NULL){
        printf("alloc error\n");
        exit(1);
    }
    
    int n=0;
    while((dent=readdir(dp))){
        stat(dent->d_name, &statbuf);
        enum TYPE type;
        printf("%s %d\n",dent->d_name, statbuf.st_mode);

        if (S_ISLNK(statbuf.st_mode)) {
            type = LINK;
        } else if (S_ISDIR(statbuf.st_mode)) {
            type = DIRR;
        } else if (S_ISREG(statbuf.st_mode)) {
            type = NORMAL;
        } else if (S_ISCHR(statbuf.st_mode)) {
            type = C_DEV;
        } else if (S_ISBLK(statbuf.st_mode)) {
            type = B_DEV;
        } else {
            type = MISSING;
        }
        contents[n].name = strdup(dent->d_name);
        contents[n].size = statbuf.st_size;
        contents[n].type = type;
        contents[n].time = statbuf.st_ctime;
        n++;
    }

    printf("\"%s\" 경로 파일 출력\n",cwd);
    if(!strcmp("size", a)){
        printf("크기로 정렬\n");
        qsort(contents, size, sizeof(struct Content), compareBySize);
    }
    else if(!strcmp("name", a)){
        printf("이름으로 정렬\n");
        qsort(contents, size, sizeof(struct Content), compareByName);
    }
    else if(!strcmp("type", a)){
        printf("타입으로 정렬\n");
        qsort(contents, size, sizeof(struct Content), compareByType);
    }
    else if(!strcmp("time", a)){
        printf("시간으로 정렬\n");
        qsort(contents, size, sizeof(struct Content), compareByTime);
    }
    else{
        printf("해당 데이터로 정렬할 수 없습니다\n");
        free(cwd);
        for(int i=0; i<size; i++)
            free(contents[i].name);
        free(contents);
        exit(1);
    }

    printf("%-15s %-15s %-15s %-3s\n", "NAME", "SIZE", "TIME", "TYPE");
    for(int i=0; i<size; i++){
        print_content(&(contents[i]));
        free(contents[i].name);
    }
    
    free(contents);
    free(cwd);
}

void print_sub_dir(){
    // char *cwd;
    DIR *dp = opendir(".");
    struct dirent* dent;
    struct stat statbuf;

    printf("-하위 디렉토리 출력-\n");
    while ((dent = readdir(dp))){
        stat(dent->d_name, &statbuf);
        if(S_ISDIR(statbuf.st_mode) && (strcmp(dent->d_name,".")) && (strcmp(dent->d_name,".."))){
            if(chdir(dent->d_name)){
                perror("chdir");
            }
            // printf("%s\n",getcwd(NULL, BUFSIZ));
            if(L_MOD){
                print_list();
            }
            if(S_MOD){
                print_sort_list(SORT_BY);
            }
            if(chdir("..")){
                perror("chdir");
            }
        }
    }

    closedir(dp);
}

void ignite(){
    if(H_MOD){
        print_header();
    }
    if(S_MOD){
        L_MOD = 0;
        print_sort_list(SORT_BY);
    }
    if(L_MOD){
        print_list();
    }
    if(R_MOD){
        print_sub_dir();
    }
}