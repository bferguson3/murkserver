// makejsonres.c
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
    char *fp = argv[1];
    if (fp[strlen(fp) - 1] != '/')
    {
        strcat(fp, "/");
    }
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir(fp)) != NULL)
    {
        while ((ent = readdir(dir)) != NULL)
        {
            if (strcmp(ent->d_name, ".") != 0)
                if (strcmp(ent->d_name, "..") != 0)
                    printf("#include \"%s%s\"\n", fp, ent->d_name);
        }
        closedir(dir);
    }
    else
    {
        perror("Error opening directory");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
