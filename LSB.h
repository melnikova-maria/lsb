#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

//функция перевода слова в двоичное представление
int* string_to_binary(char s[], int len)
    {
        if(s == NULL)
            return 0;

        int i;
        int ii = 0;
        int kk = 0;

        char *binary_char = malloc(len + 1); //указатель на область памяти
        int *binary_int = malloc(len + 1);

        binary_char[0] = '\0';

        for(i = 0; i < len; ++i)
            {
                char ch = s[i];
                for(int j = 7; j >= 0; --j)
                    {
                        if(ch & (1 << j))
                            {
                                strcat(binary_char,"1");
                            }
                        else
                            {
                                strcat(binary_char,"0");
                            }
                    }
            }

        for(ii = 0, kk = 0; ii < len; ii++, kk++)
            {
                binary_int[kk] = binary_char[ii];
                binary_int[kk] -= 48;
            }

        return binary_int; //возвращаем указатель
    }

//запрос данных у пользователя
data data_fun (data dd_fun)
    {
        int i = 0;
        char message[] = "hello";
        char scan_word[] = {};
        char* path = "BLK_24.BMP";

        // printf("word you want to encode: ");
        // scanf("%s", message);

        // printf("\npath to the image you want to encode the word into: ");
        // scanf("%s", path);

        for(i = 0; i < ((sizeof(message)/sizeof(char)) - 1); i++)
            {
                scan_word[i] = message[i];
            }
        
        int len_of_word = (sizeof(message) - 1)/sizeof(char) * 8;
        int* word = string_to_binary(scan_word, len_of_word);

        dd_fun.p = path;
        dd_fun.l = len_of_word;
        dd_fun.w = word;
        
        return dd_fun;
    }

//открытие файла и обработка его заголовка
from_header header_fun(from_header fh_fun, data dd_main_after_fun)
    {
        #pragma pack(push, 1) // установили размер выравнивания структуры в 1 байт
        typedef struct
            {
                short  bfType;     // 0x4d42 ("BM" в ASCII)
                int    bfSize;     // размер файла в байтах
                int    bfReserved; // 0
                int    bfOffBits;  // смещение до поля данных, обычно 54 = 14 + biSize
                int    biSize;     // размер струкуры в байтах:
                                   // 40(BITMAPINFOHEADER)
                int    biWidth;     // ширина в пикселах
                int    biHeight;    // высота в пикселах
                short  biPlanes;    // всегда должно быть 1 
                short  biBitCount;  //0|1|4|8|16|24|32
                int    biCompression;   // тип компрессии (0 - несжатое изображение)
                int    biSizesteam;     // Количество байт в поле данных
                int    biXPelsPerMeter; // горизонтальное разрешение, точек на метр
                int    biYPelsPerMeter; // вертикальное разрешение, точек на метр
                int    biClrUsed; // количество используемых цветов
                                // (0 - максимально возможное для данной глубины цвета)
                int    biClrImportant; // количество существенных цветов, можно считать, просто 0.
            } BMPheader;
        #pragma pack(pop) // отключили выравнивание структуры

        FILE *steam;
        steam = fopen(dd_main_after_fun.p, "r+b"); //открыли файл на чтение (поток)
        if (steam == NULL) //обработка ошибки, если файл не открывается
            {
                printf("cannot open file \n");
                exit (1);
            }

        BMPheader bh;
        size_t res;

        res = fread(&bh, 1, sizeof(BMPheader), steam); // считали заголовок файла

        //ПРОВЕРКИ
        if (res != sizeof(BMPheader)) // проверка, что считали именно заголовок
            {
                printf("\nheader of file read error\n");
                fclose(steam);
                exit(0);
            }
 
        if (bh.bfType!=0x4d42) // проверка, что тип файла "BM"
            {
                fclose(steam);
                exit(0);
            }

        fseek(steam, 0, SEEK_END);      // устанавливаем указатель на конец файл
        int file_size = ftell(steam);   // получаем значение указателя относительно начала

        // восстановим указатель в файле на область данных
        fseek(steam, sizeof(BMPheader), SEEK_SET); // SEEK_SET – смещение отсчитывается от начала файла

        //проверка, что файл - BMP файл
        if (bh.bfSize != file_size ||
            bh.bfReserved != 0 ||
            bh.bfOffBits != 14 + bh.biSize ||
            bh.biSize != 40 ||
            bh.biPlanes != 1 ||
            bh.biCompression != 0)
            {
                printf("\nthis file is not a BMP image\n");
                fclose(steam);
                exit(0);
            }

        fseek(steam, sizeof(BMPheader), SEEK_SET);

        fh_fun.s = steam;
        fh_fun.size = sizeof(BMPheader);

        return fh_fun;
    }

//запись необходимых битов
void writing_to_file(FILE *steam, int size_of_BMPheader, int bit, int ii)
    {

        char change;    //изменяемый байт
        int remainder;  //вспомогательная переменная
        int value = 0;  //число, полученное из массива (двоичный вид)
        int value_dec = 0; //число, полученное из массива (десятичный вид)
        int i = 1;

        fseek(steam, size_of_BMPheader + ii, SEEK_SET);
        change = getc(steam); //сдвигает указатель на 1 вперед
        int change_int[8];

        for (i = 0; i < 8; i++)
            {
                if ((change & (char)pow(2, 7 - i)) != 0)
                    change_int[i] = 1;
                else 
                    change_int[i] = 0;
                printf("\nchange_int[%d] %d", i, change_int[i]);
            }

        //проверка на конец файла или ошибку чтения
        if (change == EOF)
        {
            //кончился файл
            if (feof (steam) != 0)
                {
                    printf ("reading is finished\n");
                    exit(0);
                }
            //ошибка чтения
            else
                {
                    printf ("reading error\n");
                    exit(0);
                }
        }

        // изменение последнего бита
        if (change_int[7] != bit)
            {
                change_int[7] = bit;
            }

        //элементы массива схлопывает в одно число
        for (i = 0; i < 8; ++i)
            {
                value *= 10;
                value += change_int[i];
            }

        //перевод в десятичный вид
        i = 0;
        while ( value > 0)  
            { 
                remainder = value % 10;
                value /= 10;
                value_dec += remainder * pow(2, i);
                ++i;
            }
        
        fseek(steam, size_of_BMPheader + ii, SEEK_SET);
        int eof_putc = putc(value_dec, steam); //сдвигает указатель на 1 вперед

        if (eof_putc == EOF)
            printf("writing error\n");
    }

