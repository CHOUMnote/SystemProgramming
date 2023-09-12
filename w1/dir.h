void print_list();

void print_header();

void print_error();

void print_sort_list(char*);

enum TYPE {DIRR=1, LINK, C_DEV, B_DEV, NORMAL, MISSING};
struct Content{
    char* name;
    long size;
    enum TYPE type;
    long time;
};

void print_content(struct Content*);

void print_sub_dir();

void ignite();