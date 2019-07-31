#include "apue.h"

int main(int argc, char *argv[]) {
    int i;
    struct stat buff;

    for (i = 1; i < argc; i++) {
        printf("%s: ", argv[i]);
        if (lstat(argv[i], &buff) < 0) {
            err_ret("lstat error");
            continue;
        }

        if (S_ISREG(buff.st_mode))
            printf("regular");
        else if (S_ISDIR(buff.st_mode))
            printf("directory");
        else if (S_ISCHR(buff.st_mode)) 
            printf("character special");
        else if (S_ISBLK(buff.st_mode))
            printf("block special");
        else if (S_ISFIFO(buff.st_mode))
            printf("fifo");
        else if (S_ISLNK(buff.st_mode))
            printf("symbolic link");
        else if (S_ISSOCK(buff.st_mode))
            printf("socket");
        else
            printf("** unknow mode **");
        printf("\n");
    }

    exit(EXIT_SUCCESS);
}