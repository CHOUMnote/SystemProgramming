#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#define MAX 100
#define CLIENT_WRITE c_to_s[1]
#define CLIENT_READ s_to_c[0]
#define SERVER_WRITE s_to_c[1]
#define SERVER_READ c_to_s[0]

#define ST students[i]
#define NO_WARNING_GETS              \
    {                                \
        fgets(str, BUFSIZ, stdin);   \
        str[strlen(str) - 1] = '\0'; \
    }

struct STUDENT {
    int s_id;
    char s_name[32];
    int s_age;
    char s_addr[32];
    int s_grade;
};

pid_t pid;
int s_to_c[2]; // server -> client pipe
int c_to_s[2]; // client -> server pipe

void do_client() {
    char buffer[BUFSIZ];
    char str[BUFSIZ];
    while (1) {
        printf("입력 : ");
        NO_WARNING_GETS
        write(CLIENT_WRITE, str, BUFSIZ);

        int n = read(CLIENT_READ, buffer, BUFSIZ);

        if (n > 0) {
            printf("client : %s\n", buffer);
        }
    }
}

void do_server() {
    char buffer[BUFSIZ];
    char out_string[BUFSIZ];
    struct STUDENT students[MAX];
    int page[MAX] = {
        0,
    }; // poll 방식 테이블
    int size = 0;

    while (1) {
        int n = read(SERVER_READ, buffer, BUFSIZ);
        if (size == MAX)
            write(SERVER_WRITE, "공간이 부족합니다.", 10);

        else if (n > 0) {
            char flag;
            int is_exist = 0;
            char what[4];
            int serch_id = -1;
            char serch_name[32];

            sscanf(buffer, "%c", &flag);
            sscanf(buffer, "%*c %c%c%c%c", &what[0], &what[1], &what[2], &what[3]);

            if (flag == 's') {
                if (what[0] == 'i' && what[1] == 'd') {
                    sscanf(buffer, "%*s %*s %d", &serch_id);
                    if (serch_id == -1)
                        break;

                    for (int i = 0; i < MAX; i++) {
                        if (page[i] == 1 && ST.s_id == serch_id) {
                            sprintf(out_string, "id : %d\nname : %s\nage : %d\naddr : %s\ngrade : %d",
                                    ST.s_id, ST.s_name, ST.s_age, ST.s_addr, ST.s_grade);
                            is_exist = 1;
                            break;
                        }
                    }

                    if (is_exist)
                        write(SERVER_WRITE, out_string, BUFSIZ);
                    else
                        write(SERVER_WRITE, "해당 id를 갖는 학생은 없습니다.", BUFSIZ);
                } else if (what[0] == 'n' && what[1] == 'a' && what[2] == 'm' && what[3] == 'e') {
                    int cnt = 0;
                    sscanf(buffer, "%*c %*s %s", serch_name);

                    for (int i = 0; i < MAX; i++) {
                        if (page[i] == 1 && strcmp(ST.s_name, serch_name) == 0) {
                            cnt++;
                        }
                    }
                    sprintf(out_string, "%d 명의 데이터가 존재합니다.", cnt);
                    write(SERVER_WRITE, out_string, BUFSIZ);
                } else {
                    write(SERVER_WRITE, "잘못된 서식 입니다.", BUFSIZ);
                }
            } else if (flag == 'a') {
                int id = -1;
                int age = -1;
                int grade = -1;
                char name[32] = "";
                char addr[32] = "";
                sscanf(buffer, "%*s %d %s %d %s %d", &id, name, &age, addr, &grade);

                int is_dup = 0;
                for(int i=0; i<MAX; i++)
                    if (page[i] == 1 && ST.s_id == id)
                        is_dup = 1;
                

                if(is_dup){
                    //what todo?
                }
                else if (id != -1 && age != -1 && grade != -1 && strcmp(name, "") != 0 && strcmp(addr, "") != 0) {
                    for (int i = 0; i < MAX; i++) {
                        if (page[i] == 0) {
                            ST.s_id = id;
                            ST.s_age = age;
                            ST.s_grade = grade;
                            strcpy(ST.s_name, name);
                            strcpy(ST.s_addr, addr);
                            page[i] = 1;
                            size++;
                            is_exist = 1;
                            sprintf(out_string, "id : %d\nname : %s\nage : %d\naddr : %s\ngrade : %d",
                                    ST.s_id, ST.s_name, ST.s_age, ST.s_addr, ST.s_grade);
                            break;
                        }
                    }
                }

                if (is_exist)
                    write(SERVER_WRITE, out_string, BUFSIZ);
                else if(is_dup)
                    write(SERVER_WRITE, "이미 존재하는 데이터입니다.", BUFSIZ);
                else
                    write(SERVER_WRITE, "데이터를 입력하지 못하였습니다.", BUFSIZ);

            } else if (flag == 'd') {
                int cnt = 0;

                if (what[0] == 'i' && what[1] == 'd') {
                    sscanf(buffer, "%*c %*s %d", &serch_id);
                    if (serch_id == -1)
                        break;

                    for (int i = 0; i < MAX; i++) {
                        if (page[i] == 1 && ST.s_id == serch_id) {
                            page[i] = 0;
                            cnt++;
                            size--;
                            break;
                        }
                    }
                } else if (what[0] == 'n' && what[1] == 'a' && what[2] == 'm' && what[3] == 'e') {
                    sscanf(buffer, "%*c %*s %s", serch_name);
                    for (int i = 0; i < MAX; i++) {
                        if (page[i] == 1 && strcmp(ST.s_name, serch_name) == 0) {
                            page[i] = 0;
                            cnt++;
                            size--;
                        }
                    }
                }

                sprintf(out_string, "%d 개 데이터 삭제", cnt);
                write(SERVER_WRITE, out_string, BUFSIZ);
            } else {
                break;
            }
        }
    }

    printf("server : 종료\n");
    kill(pid, SIGINT);
}

int make_pipe() {
    if (pipe(s_to_c) || pipe(c_to_s))
        return 1;

    return 0;
}

int main() {
    if (make_pipe())
        perror("pipe");

    pid = fork();
    if (pid == 0) {
        // todo child
        close(s_to_c[1]);
        close(c_to_s[0]);
        do_client();
    } else if (pid > 0) {
        // todo parents
        close(s_to_c[0]);
        close(c_to_s[1]);
        do_server();
    } else {
        // todo error
        perror("fork");
    }

    return 0;
}