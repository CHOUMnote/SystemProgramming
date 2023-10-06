#pragma once
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

typedef unsigned long ul_t;
typedef unsigned int ui_t;

void ignite();
void cpu_init();
void mem_init();
void disk_init();
void net_init();
void dev_init();
void stat_init();
void proc_init();

struct CPU_INFO{
    int core_count;
    double usage;
};

struct ACCESS_MEM{
    ul_t total;
    ul_t free;
};

//meminfo()
struct MEM_INFO{
    ul_t total_mem;
    ul_t free_mem;
    // ul_t Inactive;   
    ul_t available_mem;
    ul_t buffer_mem;      //meta 페이지 캐시
    ul_t cached_mem;      //real 페이지 캐시
    ul_t swap_mem;        //스왑 메모리
    ul_t swap_free_mem;   //스왑 가능 메모리
    ul_t sh_mem;
    ul_t dirty_mem;       //dirty pages
    // struct ACCESS_MEM user_area;
    // struct ACCESS_MEM kernel_area;
};

struct DISK_INFO{
    int major_number;
    int minor_number;
    char name[32];
    int fields[17];
};

struct DEV_INFO{
    int char_dev;
    int block_dev;
};

struct NET_INFO{
    char name[32];
    ul_t r_byte;
    ul_t r_packet;
    ul_t t_byte;
    ul_t t_packet;
};
struct TCP_INFO{
  char loc[24];
  char loc_port[16];
  char rem[24];
  char rem_port[16];  
};

struct PROC_INFO{
    pid_t pid;
    pid_t ppid;
    char name[32];
    uid_t uid;
    gid_t gid;
    char state;
    ui_t pr;
    ui_t ni;
    size_t vsize;
};

struct STAT_INFO{
    ui_t ctxt; //문맥 전환 수
    time_t btime; //부팅시간
    ui_t procs_running;
    ui_t procs_blocked;
};

int _proc_init(char*, struct PROC_INFO*);
