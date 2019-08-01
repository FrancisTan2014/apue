#include "apue.h"

int main(void) {
    struct stat buf;

    /* turn on set-group-ID and turn off group-execute */
    if (stat("foo", &buf) < 0) {
        err_sys("stat error for foo");
    }
    if (chmod("foo", (buf.st_mode & ~S_IXGRP) | S_ISGID) < 0) {
        err_sys("chmod error for foo");
    }

    /* set absolute mode to "rw-r--r--" */
    if (chmod("bar", S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH) < 0) {
        err_sys("chmod error for bar");
    }

    exit(EXIT_SUCCESS);
}