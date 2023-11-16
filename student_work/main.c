#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "main.h"

#define DATA "student.data"
#define TEMP_DATA "student.data.temp"

FILE* file;
struct Student student = {-1, "NULL", -1, -1, -1};
char buffer[BUFSIZ];

void apply_file(){
    if (remove(DATA) != 0) {
        perror("원본 파일 삭제 실패");
        exit(1);
    }
    if (rename(TEMP_DATA, DATA) != 0) {
        perror("임시 파일 이름 변경 실패");
        exit(1);
    }
}

bool is_valid(int id) {
    FILE* file = fopen(DATA, "r");

    if (file == NULL) {
        perror("파일 열기 실패");
        return false;
    }

    while (fscanf(file, "%d %s %d %d %d\n", &student.id, student.name, &student.korean, &student.eng, &student.math) != -1) { //EOF == -1
        if (student.id == id) {
            fclose(file);
            return false; // 중복된 ID가 발견됨
        }
    }

    fclose(file);
    return true; // 중복된 ID 없음
}

void loading(){
    int line_count = 0;

    file = fopen(DATA, "r");
    if(file == NULL){
        file = fopen(DATA, "w+");
        if(file == NULL){
            perror("파일 열기 실패");
            exit(1);
        }
    }

    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        line_count++;
    }
    printf("%d명의 데이터 존재\n",line_count);
    fclose(file);
}

void input(){
    int id;
    printf("학생 ID를 입력하세요 : ");
    scanf("%d", &id);

    if (!is_valid(id)) {
        printf("중복 ID입니다.\n");
        return;
    }
    student.id = id;

    while (getchar() != '\n');
    printf("학생 이름을 입력하세요 (최대 29자) : ");
    fgets(student.name, sizeof(student.name), stdin);
    int len = strlen(student.name);
    if (len > 0 && student.name[len - 1] == '\n') 
        student.name[len - 1] = '\0';

    printf("국어 점수를 입력하세요 : ");
    scanf("%d", &student.korean);

    printf("영어 점수를 입력하세요 : ");
    scanf("%d", &student.eng);

    printf("수학 점수를 입력하세요 : ");
    scanf("%d", &student.math);

    file = fopen(DATA, "a");

    if (file == NULL) {
        perror("파일 열기 실패");
        exit(1);
    }

    fprintf(file, "%d %s %d %d %d\n", student.id, student.name, student.korean, student.eng, student.math);

    fclose(file);
}

void modify(){
    if(search_id()){
        FILE *file = fopen(DATA, "r");
        if (file == NULL){
            perror("파일 열기 실패");
            exit(1);
        }

        FILE *tempFile = fopen(TEMP_DATA, "w");
        if (tempFile == NULL) {
            perror("임시 파일 열기 실패");
            exit(1);
        }

        struct Student current_student;
        printf("국어 영어 수학 점수를 입력하세요 : ");
        scanf("%d %d %d", &current_student.korean, &current_student.eng, &current_student.math);
        strcpy(current_student.name, student.name);
        current_student.id = student.id;
        
        while (fscanf(file, "%d %s %d %d %d\n", &student.id, student.name, &student.korean, &student.eng, &student.math) != -1) {
            if (student.id == current_student.id) 
                continue; // 해당 ID의 학생을 건너뜁니다.
            fprintf(tempFile, "%d %s %d %d %d\n", student.id, student.name, student.korean, student.eng, student.math);
        }
        fprintf(tempFile, "%d %s %d %d %d\n", current_student.id, current_student.name, current_student.korean, current_student.eng, current_student.math);
        apply_file();
        fclose(file);
        fclose(tempFile);
    }
}

void delete(){
    FILE *file = fopen(DATA, "r");
    if (file == NULL){
        perror("파일 열기 실패");
        exit(1);
    }

    FILE *tempFile = fopen(TEMP_DATA, "w");
    if (tempFile == NULL) {
        perror("임시 파일 열기 실패");
        exit(1);
    }

    bool is_exist = false;
    int id;

    printf("id를 입력하세요 : ");
    scanf("%d", &id);
    
    while (fscanf(file, "%d %s %d %d %d\n", &student.id, student.name, &student.korean, &student.eng, &student.math) != -1) {
        if (student.id == id) {
            is_exist = true;
            continue; // 해당 ID의 학생을 건너뜁니다.
        }
        fprintf(tempFile, "%d %s %d %d %d\n", student.id, student.name, student.korean, student.eng, student.math);
    }

    fclose(file);
    fclose(tempFile);

    apply_file();

    if (!is_exist) {
        printf("해당 ID의 학생을 찾을 수 없습니다.\n");
    }
}

void save(){ }

int search_id(){
    FILE* file = fopen(DATA, "r");
    int id = -1;

    if(file == NULL){
        perror("파일 열기 실패");
        exit(1);
    }
    
    printf("id를 입력하세요 : ");
    scanf("%d", &id);
    while(fscanf(file, "%d %s %d %d %d\n", &student.id, student.name, &student.korean, &student.eng, &student.math) != -1){
        if(student.id == id){
            printf("%d %s  : %5d점%5d점%5d점\n", student.id, student.name, student.korean, student.eng, student.math);
            fclose(file);
            return 1;
        }
    }
    fclose(file);
    printf("존재하지 않는 id입니다.\n");
    return 0;
}

void search_name(){
    FILE* file = fopen(DATA, "r");
    char name[30];

    if(file == NULL){
        perror("파일 열기 실패");
        exit(1);
    }

    printf("이름을 입력하세요 : ");
    scanf("%s", name);
    while(fscanf(file, "%d %s %d %d %d\n", &student.id, student.name, &student.korean, &student.eng, &student.math) != -1){
        if(!strcmp(name, student.name)){
            printf("%d %s  : %5d점 %5d점 %5d점\n", student.id, student.name, student.korean, student.eng, student.math);
        }
    }
    fclose(file);
}

int main(){
    int n;
    while(1){
        show_menu();
        scanf("%d",&n);
        if(n==1)
            loading();
        else if(n==2)
            input();
        else if(n==3)
            modify();
        else if(n==4)
            delete();
        else if(n==5)
            save();
        else if(n==6){
            int c;
            printf("id검색 : 1, 이름 검색 : 2\n");
            scanf("%d", &c);
            if(c == 1)
                search_id();
            else if(c == 2)
                search_name();
        }
        else{
            printf("EXIT\n");
            break;
        }
    }
}