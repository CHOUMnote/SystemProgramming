#include <time.h>
#include <dirent.h>

#include "sysinfo.h"

#define KB_TO_MB(a) (a / 1024) 
#define B_TO_MB(a) (a / (1024 * 1024))

struct CPU_INFO cpu_info;
struct MEM_INFO mem_info;
struct DISK_INFO disk_info[10];
struct DEV_INFO dev_info;
struct NET_INFO net_info[10];
struct TCP_INFO tcp_info[20];
struct PROC_INFO proc_info[100];
struct STAT_INFO stat_info;

int disk_count = 0;
int net_count = 0;
int tcp_count = 0;
int proc_count = 0;

int compareByPID(const void *a, const void *b) {
    const struct PROC_INFO *contentA = (const struct PROC_INFO *)a;
    const struct PROC_INFO *contentB = (const struct PROC_INFO *)b;
    return contentA->pid - contentB->pid;
}

void print_info(){
    //stat
    char time_str[64];
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", localtime(&stat_info.btime));

    printf("- 시스템 정보\n");
    printf("문맥 전환:%u 부팅 시간:%s running:%u blocked:%u\n",
    stat_info.ctxt, time_str, stat_info.procs_running, stat_info.procs_blocked);
    printf("\n");

    //cpu
    printf("- CPU 정보\n");
    printf("코어%d개 - %.2lf%% 사용중\n", cpu_info.core_count, cpu_info.usage);
    printf("\n");

    //mem
    ul_t buff_cache = mem_info.buffer_mem + mem_info.cached_mem;
    ul_t usage_mem = mem_info.total_mem - (mem_info.free_mem + buff_cache + mem_info.sh_mem);
    ul_t usage_swap_mem = mem_info.swap_mem - mem_info.swap_free_mem;

    printf("- 메모리 정보\n");
    printf("%-8s %12s %12s %12s %12s %12s %12s (MB)\n"," ","total","used","free","buff/cache","shared","available");
    printf("%-8s %12lu %12lu %12lu %12lu %12lu %12lu \n", "(Mem)",
    KB_TO_MB(mem_info.total_mem), KB_TO_MB(usage_mem), KB_TO_MB(mem_info.free_mem),
    KB_TO_MB(buff_cache), KB_TO_MB(mem_info.sh_mem), KB_TO_MB(mem_info.available_mem));
    printf("%-8s %12lu %12lu %12lu \n", "(Swap)", 
    KB_TO_MB(mem_info.swap_mem), KB_TO_MB(usage_swap_mem), KB_TO_MB(mem_info.swap_free_mem));
    printf("\n");

    //disk
    int total_r = 0;    //1
    int total_w = 0;    //5
    int read_s = 0;     //3
    int write_s = 0;    //7
    int read_time = 0;  //4
    int write_time = 0; //8

    for(int i=0; i<disk_count; i++){
        total_r += disk_info[i].fields[0];
        total_w += disk_info[i].fields[4];
        read_s += disk_info[i].fields[2];
        write_s += disk_info[i].fields[6];
        read_time += disk_info[i].fields[3];
        write_time += disk_info[i].fields[7];
    }

    printf("- DISK 정보\n");
    printf("%-8s %12s %12s %12s %12s %12s %12s (MB)\n"," ","read","write","r_sector","w_sector","r_time","w_time");
    for(int i=0; i<disk_count; i++){
        printf("%-8s %12u %12u %12u %12u %12u %12u \n", disk_info[i].name,
            disk_info[i].fields[0], disk_info[i].fields[4], disk_info[i].fields[2],
            disk_info[i].fields[6], disk_info[i].fields[3], disk_info[i].fields[7]);
    }
    printf("%-8s %12u %12u %12u %12u %12u %12u \n", "Total",
        total_r, total_w, read_s, write_s, read_time, write_time);
    printf("\n");

    //net
    printf("- NETWORK 정보\n");
    printf("%-8s %12s %12s %12s %12s \n","interface","r_bytes","r_packets","t_bytes","t_packets");
    for (int i = 0; i < net_count; i++) {
        printf("%-8s %12lu %12lu %12lu %12lu \n", 
        net_info[i].name, net_info[i].r_byte, net_info[i].r_packet,
        net_info[i].t_byte, net_info[i].t_packet);
    }
    //tcp
    printf("TCP listen...\n");
    for (int i = 0; i < tcp_count; i++) {
        printf("%s:%s --> %s:%s \n", tcp_info[i].loc, tcp_info[i].loc_port, tcp_info[i].rem, tcp_info[i].rem_port);
    }
    printf("\n");

    //dev
    printf("- 장치 정보\n");
    printf("Character devices : %d개 \t Block devices : %d개\n", dev_info.char_dev, dev_info.block_dev);
    printf("\n");

    //proc
    printf("%-6s %-6s %-4s %-6s %-6s %-4s %-4s %-14s %-s \n","PID","PPID","S","UID", "GID", "PR", "NI", "VIRT", "NAME");
    for(int i=0; i<proc_count; i++){
        printf("%-6d %-6d %-4c %-6d %-6d %-4d %-4d %-8ld %s %-s \n",
        proc_info[i].pid, proc_info[i].ppid, proc_info[i].state, proc_info[i].uid, proc_info[i].gid, 
        proc_info[i].pr, proc_info[i].ni, B_TO_MB(proc_info[i].vsize), "(MB)", proc_info[i].name);
    }
}

void ignite(){
    cpu_init(); //cpu 정보 초기화
    mem_init();
    disk_init();
    net_init();
    dev_init();
    stat_init();
    proc_init();

    print_info();
}

void cpu_init() {
    char line[1024];
    char cpu[10];
    unsigned long long user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice;

    //cpu 코어 갯수
    FILE *cpuinfo = fopen("/proc/cpuinfo", "r");
    if (cpuinfo == NULL) {
        perror("파일 열기 실패");
        return;
    }
    int core_count = 0;

    while (fgets(line, sizeof(line), cpuinfo)) {
        if (strstr(line, "processor")) {
            core_count++;
        }
    }
    fclose(cpuinfo);
    cpu_info.core_count = core_count;
    
    //cpu 세부정보
    FILE* stat = fopen("/proc/stat", "r");
    if (stat == NULL) {
        perror("파일 열기 실패");
        return;
    }
    
    while (fgets(line, sizeof(line), stat) != NULL) {  
    /* htop - ProcessList.c
    usertime = usertime - guest;
    nicetime = nicetime - guestnice;
    // Fields existing on kernels >= 2.6
    // (and RHEL's patched kernel 2.4...)
    idlealltime = idletime + ioWait;
    systemalltime = systemtime + irq + softIrq;
    virtalltime = guest + guestnice;
    totaltime = usertime + nicetime + systemalltime + idlealltime + steal + virtalltime;
    */
        if (strncmp(line, "cpu ", 4) == 0) {    //cpu 총합 정보
            sscanf(line, "%s %llu %llu %llu %llu %llu %llu %llu %llu %llu %llu",
                   cpu, &user, &nice, &system, &idle, &iowait, &irq, &softirq, &steal, &guest, &guest_nice);

            unsigned long long idle_time = idle + iowait;
            unsigned long long non_idle_time = user + nice + system + irq + softirq + steal;
            unsigned long long total = idle_time + non_idle_time;
            cpu_info.usage = ((double)(total - idle) / total) * 100.0;
        } else {
            break;
        }
    }
    fclose(stat);
}

void mem_init() {
    FILE *meminfo = fopen("/proc/meminfo", "r");
    if (meminfo == NULL) {
        perror("파일 열기 실패");
        return;
    }

    char line[1024];
    unsigned long cap;

    while (fgets(line, sizeof(line), meminfo) != NULL) { 
        //struct setting
        if(sscanf(line, "MemTotal: %lu kB", &cap)){
            mem_info.total_mem = cap;
            continue;
        }
        if(sscanf(line, "MemFree: %lu kB", &cap)){
            mem_info.free_mem = cap;
            continue;
        }
        if(sscanf(line, "MemAvailable: %lu kB", &cap)){
            mem_info.available_mem = cap;
            continue;
        }
        if(sscanf(line, "Buffers: %lu kB", &cap)){
            mem_info.buffer_mem = cap;
            continue;
        }
        if(sscanf(line, "Cached: %lu kB", &cap)){
            mem_info.cached_mem = cap;
            continue;
        }
        if(sscanf(line, "SwapTotal: %lu kB", &cap)){
            mem_info.swap_mem = cap;
            continue;
        }
        if(sscanf(line, "SwapFree: %lu kB", &cap)){
            mem_info.swap_free_mem = cap;
            continue;
        }
        if(sscanf(line, "Shmem: %lu kB", &cap)){
            mem_info.sh_mem = cap;
            continue;
        }
        if(sscanf(line, "Dirty: %lu kB", &cap)){
            mem_info.dirty_mem = cap;
            continue;
        }
    }

    fclose(meminfo);
}

void disk_init(){
    FILE* diskstats = fopen("/proc/diskstats", "r");
    if(diskstats == NULL){
        perror("파일 열기 실패");
        return;
    }

    char line[1024];
    char name[32];
    int a,b,c[17];
    while(fgets(line, sizeof(line), diskstats)){
        sscanf(line, "%d %d %s %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d", 
        &a, &b, name, &c[0], &c[1], &c[2], &c[3], &c[4], &c[5], &c[6], &c[7], &c[8], &c[9], &c[10],
        &c[11], &c[12], &c[13], &c[14], &c[15], &c[16]); 

        if(strncmp(name, "sd", 2) == 0){
            disk_info[disk_count].major_number = a;
            disk_info[disk_count].minor_number = b;
            strncpy(disk_info[disk_count].name, name, sizeof(name));
            memmove(disk_info[disk_count].fields, c, sizeof(int)*17);

            disk_count++;
        }
    }

    fclose(diskstats);
}

void net_init(){
    FILE *dev = fopen("/proc/net/dev", "r");
    if (dev == NULL) {
        perror("파일 열기 실패");
        return;
    }

    char line[1024];

    //헤더 넘기기
    for (int i = 0; i < 2; i++) 
        fgets(line, sizeof(line), dev);
    
    while (fgets(line, sizeof(line), dev)) {
        char name[32];
        unsigned long r_bytes, r_packets, t_bytes, t_packets;
        if (sscanf(line, "%s %lu %lu %*d %*d %*d %*d %*d %*d %lu %lu", name, &r_bytes, &r_packets, &t_bytes, &t_packets) == 5) {
            strncpy(net_info[net_count].name, name, sizeof(name));
            net_info[net_count].r_byte = r_bytes;
            net_info[net_count].r_packet = r_packets;
            net_info[net_count].t_byte = t_bytes;
            net_info[net_count].t_packet = t_packets;
            net_count++;
        }
    }

    fclose(dev);

    //tcp
    FILE *tcp = fopen("/proc/net/tcp", "r");
    if (tcp == NULL) {
        perror("파일 열기 실패");
        return;
    }
    
    //헤더 넘기기
    fgets(line, sizeof(line), tcp);

    while (fgets(line, sizeof(line), tcp)) {
        char loc[24], loc_port[16], rem[24], rem_port[16];  
        if (sscanf(line, "%*d: %8s:%4s %8s:%4s", loc, loc_port, rem, rem_port) == 4) {
            strncpy(tcp_info[tcp_count].loc, loc, sizeof(loc));
            strncpy(tcp_info[tcp_count].loc_port, loc_port, sizeof(loc_port));
            strncpy(tcp_info[tcp_count].rem, rem, sizeof(rem));
            strncpy(tcp_info[tcp_count].rem_port, rem_port, sizeof(rem_port));
            tcp_count++;
        }
    }

    fclose(tcp);

    return;
}

void dev_init(){
    char line[1024];

    FILE *devices = fopen("/proc/devices", "r");
        if (devices == NULL) {
        perror("파일 열기 실패");
        return;
    }

    int flag = 1;
    while (fgets(line, sizeof(line), devices)) {
        if (strstr(line, "Char")) {
            continue;
        }
        if (strstr(line, "Block")) {
            flag = 0;
        }
        if(flag)   dev_info.char_dev++;
        else       dev_info.block_dev++;
    }
    dev_info.char_dev--; //공백
    dev_info.block_dev--;

    fclose(devices);
}

void stat_init(){
    char line[1024];
    ui_t value;
    ul_t time;

    FILE* stat = fopen("/proc/stat", "r");
    if (stat == NULL) {
        perror("파일 열기 실패");
        return;
    }

    while (fgets(line, sizeof(line), stat) != NULL) {  
        if (sscanf(line, "ctxt %u", &value)) 
            stat_info.ctxt = value;
        else if (sscanf(line, "btime %lu", &time)) 
            stat_info.btime = time;
        else if (sscanf(line, "procs_running %u", &value)) 
            stat_info.procs_running = value;
        else if (sscanf(line, "procs_blocked %u", &value)) 
            stat_info.procs_blocked = value;
    }

    fclose(stat);
}

int _proc_init(char* id, struct PROC_INFO* proc){
    char line[BUFSIZ];
    char stat_path[512] = "/proc/";
    strcat(stat_path,id);
    strcat(stat_path,"/stat");
    char status_path[512] = "/proc/";
    strcat(status_path,id);
    strcat(status_path,"/status");

    FILE* stat = fopen(stat_path, "r");
    if (stat == NULL) {
        perror("파일 열기 실패");
        return 0;
    }
    
    char* form = "%d %32s %c %d %*d %*d %*d %*u %*lu %*lu %*lu %*lu %*lu %*u %*u %*d %*d %d %d %*d %*lu %*lu %lu";
    fgets(line, sizeof(line), stat);
    sscanf(line, form, 
    &(proc->pid), proc->name, &(proc->state), &(proc->ppid), &(proc->pr), &(proc->ni), &(proc->vsize));

    fclose(stat);

    FILE* status = fopen(status_path, "r");
    if (status == NULL) {
        perror("파일 열기 실패");
        return 0;
    }

    while (fgets(line, sizeof(line), status) != NULL) {  
        if (strncmp(line, "Uid:", 4) == 0) 
            sscanf(line, "Uid:\t%d", &(proc->uid));
        
        else if (strncmp(line, "Gid:", 4) == 0) 
            sscanf(line, "Gid:\t%d", &(proc->gid));
        
    }

    fclose(status);
    return 1;
}

void proc_init(){
    DIR* dir = opendir("/proc/");
    struct dirent* dent;

    while ((dent = readdir(dir))){
        int is_p = atoi(dent->d_name);
        char buf[256];
        sprintf(buf,"%d",is_p);
        if(!strcmp(buf, dent->d_name)){ //파일 검증
            strcpy(proc_info[proc_count].name, dent->d_name);
            proc_count++;
        }
    }
    closedir(dir);

    for(int i=0; i<proc_count; i++){
        if(_proc_init(proc_info[i].name, &proc_info[i])){
            //todo?
        }   
        else{
            return;
        }
    }
    qsort(proc_info, proc_count, sizeof(struct PROC_INFO), compareByPID);

}
