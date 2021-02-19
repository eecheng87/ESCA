#include "preload.h"
int curindex;
int table_size = 64;

int open(const char *pathname, int flags, mode_t mode) {

    btable[curindex].sysnum = __NR_open;
    btable[curindex].rstatus = BENTRY_BUSY;
    btable[curindex].nargs = 3;
    btable[curindex].args[0] = (long)pathname;
    btable[curindex].args[1] = flags;
    btable[curindex].args[2] = mode;
    curindex = (curindex == table_size - 1) ? 0 : curindex + 1;

    /* memorize the -index of fd */
    return -(curindex - 1);
}

int close(int fd) {

    btable[curindex].sysnum = __NR_close;
    btable[curindex].rstatus = BENTRY_BUSY;
    btable[curindex].nargs = 1;
    btable[curindex].args[0] = fd;
    curindex = (curindex == table_size - 1) ? 0 : curindex + 1;

    return 0;
}

ssize_t write(int fd, const void *buf, size_t count) {

    btable[curindex].sysnum = __NR_write;
    btable[curindex].rstatus = BENTRY_BUSY;
    btable[curindex].nargs = 3;
    btable[curindex].args[0] = fd;
    btable[curindex].args[1] = (long)buf;
    btable[curindex].args[2] = count;
    curindex = (curindex == table_size - 1) ? 0 : curindex + 1;

    return 0;
}

ssize_t read(int fd, void *buf, size_t count) {

    btable[curindex].sysnum = __NR_read;
    btable[curindex].rstatus = BENTRY_BUSY;
    btable[curindex].nargs = 3;
    btable[curindex].args[0] = fd;
    btable[curindex].args[1] = (long)buf;
    btable[curindex].args[2] = count;
    curindex = (curindex == table_size - 1) ? 0 : curindex + 1;

    return 0;
}

__attribute__((constructor)) static void setup(void) {
    size_t pgsize = getpagesize();
    curindex = 0;
    btable = (struct batch_entry*)aligned_alloc(pgsize, pgsize);
    syscall(__NR_register, btable);
}