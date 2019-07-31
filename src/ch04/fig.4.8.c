#include "apue.h"
#include <fcntl.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("usage: fig.4.3 <pathname>");
    }

    const char *fileName = argv[1];
    if (access(fileName, R_OK) < 0) {
        err_ret("access error for %s", argv[1]);
    }
    else {
        printf("read access OK\n");
    }
    if (open(fileName, O_RDONLY) < 0) {
        err_ret("open error for %s", argv[1]);
    }
    else {
        printf("open for reading OK\n");
    }

    exit(0);
}