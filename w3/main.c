#include "main.h"

// memory check
int size_check(char *str) {
    int a = BUFSIZ - strlen(addr);

    return a >= strlen(str);
}

void find_and_print(char flag, int IDX /* index */) {
    // printf("받은 데이터 : %d\n", IDX);
    if (IDX == -1) {
        printf("실패\n");
        return;
    }

    char line[BUFSIZ];
    int line_no = 0;
    int start = 0;
    int end = 0;

    switch (flag) {
    case 'a':
    case 's':
        while (addr[end] != '\0' && end < BUFSIZ) {
            if (addr[end] == '\n') {
                int size = end - start;
                if (IDX == line_no) {
                    strncpy(line, (addr + start), size);
                    line[size] = '\0';
                    printf("%s\n", line);
                }
                start = end + 1;
                line_no++;
            }
            end++;
        }
        break;
    case 'd':
        if (IDX == -1)
            printf("해당 ID는 존재하지 않습니다.\n");
        else
            printf("%d번째 행 삭제 완료\n", IDX);
        break;
    }
}

// id, name, age, local, grade
void do_client() {
    char str[BUFSIZ];
    int recived_data;
    char flag = '\0';

    while (1) {
        printf("입력 : ");
        NO_WARNING_GETS
        sscanf(str, "%c", &flag);
        write(CLIENT_WRITE, str, BUFSIZ);
        int n = read(CLIENT_READ, &recived_data, sizeof(int));

        if (n > 0) {
            // printf("client : %d\n", recived_data);
            find_and_print(flag, recived_data);
        }
    }
}

void do_server() {
    char buffer[BUFSIZ]; // command inputs
    char out_string[BUFSIZ];
    int send_data = 1;

    // student data

    while (1) {
        struct STUDENT st;
        char flag;

        int n = read(SERVER_READ, buffer, BUFSIZ);
        if (n == 0) {
            perror("read");
            exit(1);
            kill(pid, SIGINT);
        }

        sscanf(buffer, "%c", &flag);

        if (flag == 'a') {
            char line[BUFSIZ];
            struct STUDENT temp_student;

            // a [id] [name]...
            sscanf(buffer, "%*s %d %s %d %s %d", &temp_student.s_id, temp_student.s_name,
                   &temp_student.s_age, temp_student.s_addr, &temp_student.s_grade);

            // printf("origin : %s\n", buffer);
            // DEBUG_PRINT(temp_student);

            // line slice
            int start = 0;
            int end = 0;
            int val = 0;
            int IDX = 0;
            while (addr[end] != '\0' && end < BUFSIZ) {
                if (addr[end] == '\n') {
                    int size = end - start;
                    int t_id;
                    strncpy(line, (addr + start), size);
                    line[size] = '\0';
                    start = end + 1;

                    sscanfz(line, "%d", &t_id);
                    // printf("%d : %d \n", temp_student.s_id, t_id);
                    if (val = ID_VAL(temp_student.s_id, t_id)) {
                        // id 중복
                        IDX = -1;
                        break;
                    }
                    IDX++;
                }
                end++;
                val = 0;
            }

            if (val == 0) {
                sprintf(out_string, "%d %s %d %s %d\n", temp_student.s_id, temp_student.s_name,
                        temp_student.s_age, temp_student.s_addr, temp_student.s_grade);
                if (size_check(out_string))
                    strcat(addr, out_string);
            } else if (val == 1)
                printf("중복 데이터 입니다.\n");
            send_data = IDX; // index

        } else if (flag == 's') {
            char line[BUFSIZ];
            int need_id;
            sscanf(buffer, "%*s %d", &need_id); // s [id]

            // line slice
            int start = 0;
            int end = 0;
            int val = 0;
            int IDX = 0;
            while (addr[end] != '\0' && end < BUFSIZ) {
                if (addr[end] == '\n') {
                    int size = end - start;
                    int t_id;
                    strncpy(line, (addr + start), size);
                    line[size] = '\0';
                    start = end + 1;

                    sscanf(line, "%d", &t_id);
                    if (t_id == need_id) {
                        val = 1;
                        break;
                    }
                    IDX++;
                }
                end++;
            }

            if (val /* id exist??*/)
                send_data = IDX;
            else {
                send_data = -1;
                printf("존재 하지 않는 ID입니다.\n");
            }
        } else if (flag == 'd') {
            char line[BUFSIZ];
            char temp_addr[BUFSIZ];
            int need_id = -1;

            //init array
            for(int i=0; i<BUFSIZ; i++)
                temp_addr[i] = '\0';

            sscanf(buffer, "%*s %d", &need_id); // d [id]

            // line slice
            int start = 0;
            int end = 0;
            int val = 0;
            int IDX = 0;

            while (addr[end] != '\0' && end < BUFSIZ) {
                if (addr[end] == '\n') {
                    int size = end - start;
                    int t_id;
                    strncpy(line, (addr + start), size);
                    line[size] = '\0';
                    start = end + 1;

                    sscanf(line, "%d", &t_id);
                    if (t_id == need_id) {
                        val = 1;
                        send_data = IDX;
                        end++;
                        continue;
                    }
                    line[size] = '\n';
                    strncat(temp_addr, line, size + 1);
                    IDX++;
                }
                end++;
            }

            if (val /* id exist?? */)
                strncpy(addr, temp_addr, BUFSIZ);
            else {
                send_data = -1;
                printf("존재 하지 않는 ID입니다.\n");
            }
        } else {
            // end loop
            break;
        }

        write(SERVER_WRITE, &send_data, sizeof(int));
    }

    printf("server : 종료\n");
    kill(pid, SIGINT);
}

extern inline void init();
extern inline void esc();
int main() {
    init();

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

    esc();

    return 0;
}