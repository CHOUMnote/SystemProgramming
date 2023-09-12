#include <stdio.h>
#include <unistd.h>
#include "dir.h"

extern int S_MOD;
extern int H_MOD;
extern int R_MOD;
extern int L_MOD;
extern char* SORT_BY;
int main(int argc, char* argv[]){
    int n;
    char* a;

    while((n=getopt(argc,argv,"hls:r")) != -1){
        switch (n){
            case 'h':
                H_MOD = 1;
                break;
            case 'l':
                L_MOD = 1;
                break;
            case 's':
                S_MOD = 1;
                a = optarg;
                SORT_BY = a;
                break;
            case 'r':
                R_MOD = 1;
                break;
            case '?':
                print_error();
                break;
            default : 
                return 1;
        }
    }

    ignite();

	return 0;
}

