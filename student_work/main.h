#pragma once
#include <stdbool.h>

struct Student{
    int id;
    char name[30];
    int korean;
    int eng;
    int math;
};

//1.로딩 2.입력 3.수정 4.삭제 5.저장 6.검색
void show_menu(){
    char* title = "1.로딩 2.입력 3.수정 4.삭제 5.저장 6.검색 other.종료\n";
    printf("%s",title);
}

void loading();
void input();
void modify();
void delete();
void save();
int search_id();
void serch_name();

bool is_valid(int);