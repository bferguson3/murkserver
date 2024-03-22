
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

char *generate_new_guid()
{
    srand(clock());
    // char GUID[40];
    char *GUID = (char *)malloc(16);
    int t = 0;
    char *szTemp = "xxxxxxxxxxxxxxxx";  //-xxxx-4xxx-yxxx-xxxxxxxxxxxx";
    char *szHex = "0123456789abcdef-";
    int nLen = strlen(szTemp);

    for (t = 0; t < nLen + 1; t++)
    {
        int r = rand() % 16;
        char c = ' ';

        switch (szTemp[t])
        {
            case 'x':
            {
                c = szHex[r];
            }
            break;
            case 'y':
            {
                c = szHex[r & 0x03 | 0x08];
            }
            break;
            case '-':
            {
                c = '-';
            }
            break;
            case '4':
            {
                c = '4';
            }
            break;
        }

        GUID[t] = (t < nLen) ? c : 0x00;
    }
    return GUID;
    // printf ("%s\r\n", GUID);
}