#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
    FILE *f = fopen(argv[1], "r");
    if (f == NULL)
    {
        printf("Error opening file\n");
        return 1;
    }

    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);

    char *inby = malloc(fsize + 1);
    fread(inby, fsize, 1, f);
    fclose(f);

    inby[fsize] = '\0';

    char *outby = malloc((2 * fsize) + 1);
    int i = 0;
    int j = 0;
    while (i < strlen(inby))
    {
        if (inby[i] == '\"')
        {
            outby[j++] = '\\';
            outby[j++] = '\"';
        }
        else if (inby[i] == '\n')
        {
            outby[j++] = '\\';
            outby[j++] = '\n';
        }
        else
        {
            outby[j++] = inby[i];
        }
        i++;
    }
    outby[j] = '\0';

    char *filename =
        strrchr(argv[1], '/') ? strrchr(argv[1], '/') + 1 : argv[1];
    char *basename = strtok(filename, ".");

    printf("const char* %s_json = \"%s\";\n", basename, outby);

    free(inby);
    free(outby);

    return 0;
}
