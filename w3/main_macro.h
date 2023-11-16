#pragma once

#define IDX __INDEX__INT
#define str __STR_CONTEXT
#define DATA "student.txt"
#define TEMP_DATA "student.txt.temp"
#define DEBUG_PRINT(_T) printf("DEBUG : %d %s %d %s %d\n", \
                               _T.s_id, _T.s_name, _T.s_age, _T.s_addr, _T.s_grade);

#define ID_VAL(__a, __b) (__a == __b)

#define MAX 100
#define CLIENT_WRITE c_to_s[1]
#define CLIENT_READ s_to_c[0]
#define SERVER_WRITE s_to_c[1]
#define SERVER_READ c_to_s[0]

#define ST students[IDX]
#define NO_WARNING_GETS              \
    {                                \
        fgets(str, BUFSIZ, stdin);   \
        str[strlen(str) - 1] = '\0'; \
    }
