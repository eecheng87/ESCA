#include "preload.h"
int curindex = 0;
int open(const char *pathname, int flags, mode_t mode) {
    fprintf(stderr, "called open()\n");
    if (!real_open)
        real_open = dlsym(RTLD_NEXT, "open");

    return real_open(pathname, flags, mode);
}

int close(int fd) {
    fprintf(stderr, "called close\n");
    if (!real_close)
        real_close = dlsym(RTLD_NEXT, "write");

    return real_close(fd);
}

ssize_t write(int fd, const void *buf, size_t count) {
    fprintf(stderr, "called write()\n");
    /*if (!real_write)
        real_write = dlsym(RTLD_NEXT, "write");*/
    btable[curindex].sysnum = __NR_write;
    btable[curindex].nargs = 3;
    btable[curindex].args[0] = fd;
    btable[curindex].args[1] = (long)buf;
    btable[curindex++].args[2] = count;

    return curindex - 1;//real_write(fd, buf, count);
}

ssize_t read(int fd, void *buf, size_t count) {
    fprintf(stderr, "called read()\n");
    if (!real_read)
        real_read = dlsym(RTLD_NEXT, "read");

    return real_read(fd, buf, count);
}

__attribute__((constructor)) static void setup(void) {
    fprintf(stderr, "called setup()\n");
    size_t pgsize = getpagesize();
    btable = (struct batch_entry*)aligned_alloc(pgsize, pgsize);
    fprintf(stderr, "address of btable is %p, size is %ld\n", btable, pgsize);
    syscall(__NR_register, btable);
}