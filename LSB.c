#include <stdio.h>
#include "LSB.h"

typedef struct 
    {
        char* p;
        int l;
        int* w;
    } data;

typedef struct 
    {
        FILE* s;
        int size;
    } from_header;


int main()
    {
        int i, ii = 0;

        data dd_main_before;
        data dd_main_after;

        from_header fh_before;
        from_header fh_after;

        //запрос данных у пользователя
        dd_main_after = data_fun(dd_main_before);

        for (i = 0; i < 40; i++)
            {
                printf("%d", dd_main_after.w[i]);
            }

        //открытие файла и обработка его заголовка
        fh_after = header_fun(fh_before, dd_main_after);

        // запись необходимых битов
        for(i = 0; i < dd_main_after.l; i++)
            {
                writing_to_file(fh_after.s, fh_after.size, dd_main_after.w[i], ii * 3);
                ii += 1;
            }

        printf ("\nsuccess");
        return 0;
    };