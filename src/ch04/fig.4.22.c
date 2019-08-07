#include "apue.h"
#include <dirent.h>
#include <limits.h>

/* function type that is called for each filename */
typedef int Count(const char *, const struct stat *, int);

static Count count;
static int myftw(char *, Count *);
static int dopath(Count *);
static void print_counts(const char *, long, long);

static long nreg, ndir, nblk, nchr, nfifo, nslink, nsock, ntot;

int main(int argc, char *argv[]) {
    int ret;

    if (argc != 2) {
        err_quit("usage: ftw <starting-pathname>");
    }

    ret = myftw(argv[1], count);

    ntot = nreg + ndir + nblk + nchr + nfifo + nslink + nsock;
    if (ntot == 0) {
        ntot = 1; /* avoid divide by 0; print 0 for all counts */
    }
    
    print_counts("regular files", nreg, ntot);
    print_counts("directories", ndir, ntot);
    print_counts("block special", nblk, ntot);
    print_counts("char special", nchr, ntot);
    print_counts("FIFOs", nfifo, ntot);
    print_counts("symbolic links", nslink, ntot);
    print_counts("sockets", nsock, ntot);

    exit(ret);
}

void print_counts(const char *name, long cnt, long total) {
    printf("%s   = %7ld, %5.2f %%\n", name, cnt, cnt*100.0/total);
}

#define FTW_F 1 /* file other than directory */
#define FTW_D 2 /* directory */
#define FTW_DNR 3 /* directory that can't be read */
#define FTW_NS 4 /* file that we can't stat */

static int count(const char *pathname, const struct stat *statptr, int type) {
    switch (type) {
        case FTW_F:
            switch (statptr->st_mode & S_IFMT) {
                case S_IFREG: nreg++; break;
                case S_IFBLK: nblk++; break;
                case S_IFCHR: nchr++; break;
                case S_IFIFO: nfifo++; break;
                case S_IFLNK: nslink++; break;
                case S_IFSOCK: nsock++; break;
                case S_IFDIR:
                    err_dump("for S_IFDIR for %s", pathname);
            }
            break;
        case FTW_D:
            ndir++;
            break;
        case FTW_DNR:
            err_ret("can't read directory %s", pathname);
            break;
        case FTW_NS:
            err_ret("stat error for %s", pathname);
            break;
        default:
            err_dump("unknow type %d for pathname %s", type, pathname);
    }
    return 0;
}

static char *fullpath;
static size_t pathlen;

static int myftw(char *pathname, Count *func) {
    fullpath = path_alloc(&pathlen);

    if (pathlen <= strlen(pathname)) {
        pathlen = strlen(pathname) * 2;
        if ((fullpath = realloc(fullpath, pathlen)) == NULL) {
            err_sys("realloc failed");
        }
    }

    strcpy(fullpath, pathname);
    return (dopath(func));
}

/**
 * Descend through the hierarchy, starting at 'fullpath'.
 * If 'fullpath' is anything other than a directory, we lstat() it,
 * call func(), and return. For a directory, we call ourself
 * recursively for each name in the directory.
 */
static int dopath(Count *func) {
    struct stat statbuf;
    struct dirent *dirp;
    DIR *dp;
    int ret, n;

    if (lstat(fullpath, &statbuf) < 0) { /* stat error */
        return func(fullpath, &statbuf, FTW_NS);
    }
    if (S_ISDIR(statbuf.st_mode) == 0) { /* Not a directory */
        return func(fullpath, &statbuf, FTW_F);
    }

    /**
     * It's a directory. First call func() for the directory,
     * then process each filename in the directory.
     */
    if ((ret = func(fullpath, &statbuf, FTW_D)) != 0) {
        return ret;
    }

    n = strlen(fullpath);
    if (n + NAME_MAX + 2 > pathlen) { /* expand path buffer */
        pathlen *= 2;
        if ((fullpath = realloc(fullpath, pathlen)) == NULL) {
            err_sys("realloc failed");
        }
    }
    fullpath[n++] = '/';
    fullpath[n] = 0;

    if ((dp = opendir(fullpath)) == NULL) { /* cannot read directory */
        return func(fullpath, &statbuf, FTW_DNR);
    }

    while ((dirp = readdir(dp)) != NULL) {
        if (strcmp(dirp->d_name, ".") == 0 ||
            strcmp(dirp->d_name, "..") ==0) {
            continue; /* ignore dot and dot-dot */
        }
        strcpy(&fullpath[n], dirp->d_name); /* append name after "/" */
        if ((ret = dopath(func)) != 0) {
            break; /* time to leave */
        }
    }
    fullpath[n-1] = 0; /* erase everything from slash onward */

    if (closedir(dp) < 0) {
        err_ret("can't close directory %s", fullpath);
    }
    return ret;
}