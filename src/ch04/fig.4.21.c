#include "apue.h"
#include <fcntl.h>

int main(int argc, char *argv[]) {
    int i, fd;
    struct stat statbuf;
    struct timespec times[2];

    for (i = 1; i < argc; i++) {
        const char *filename = argv[i];
        if (stat(filename, &statbuf) < 0) {
            printf("%s: stat error", filename);
            continue;
        }
        if ((fd = open(filename, O_RDWR | O_TRUNC)) < 0) {
            printf("%s: open error", filename);
            continue;
        }
        times[0] = statbuf.st_atim;
        times[1] = statbuf.st_mtim;
        if (futimens(fd, times) < 0) {
            err_ret("%s: futimens error", filename);
        }
        close(fd);
    }

    exit(EXIT_SUCCESS);
}