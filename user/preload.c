#include "preload.h"
int curindex;
int table_size = 64;
int main_thread_pid;

int open(const char *pathname, int flags, mode_t mode) {
    int off = syscall(186) - main_thread_pid;
    off = off << 6;
    btable[off + curindex].sysnum = __NR_open;
    btable[off + curindex].rstatus = BENTRY_BUSY;
    btable[off + curindex].nargs = 3;
    btable[off + curindex].args[0] = (long)pathname;
    btable[off + curindex].args[1] = flags;
    btable[off + curindex].args[2] = mode;
    btable[off + curindex].pid = main_thread_pid + off;
    curindex = (curindex == table_size - 1) ? 1 : curindex + 1;

    /* memorize the -index of fd */
    return -(curindex - 1);
}

int close(int fd) {
    int off = syscall(186) - main_thread_pid;
    off = off << 6;
    btable[off + curindex].sysnum = __NR_close;
    btable[off + curindex].rstatus = BENTRY_BUSY;
    btable[off + curindex].nargs = 1;
    btable[off + curindex].args[0] = fd;
    btable[off + curindex].pid = main_thread_pid + off;
    curindex = (curindex == table_size - 1) ? 1 : curindex + 1;

    return 0;
}

ssize_t write(int fd, const void *buf, size_t count) {
    int off = syscall(186) - main_thread_pid;
    //printf("Try to wrtie at table[%d][%d]\n", off, curindex);
    off = off << 6;
    btable[off + curindex].sysnum = __NR_write;
    btable[off + curindex].rstatus = BENTRY_BUSY;
    btable[off + curindex].nargs = 3;
    btable[off + curindex].args[0] = fd;
    btable[off + curindex].args[1] = (long)buf;
    btable[off + curindex].args[2] = count;
    btable[off + curindex].pid = main_thread_pid + off;
    /* TODO: let each table has their own `curindex` */
    curindex = (curindex == table_size - 1) ? 1 : curindex + 1;

    return 0;
}

ssize_t read(int fd, void *buf, size_t count) {
    int off = syscall(186) - main_thread_pid;
    off = off << 6;
    btable[off + curindex].sysnum = __NR_read;
    btable[off + curindex].rstatus = BENTRY_BUSY;
    btable[off + curindex].nargs = 3;
    btable[off + curindex].args[0] = fd;
    btable[off + curindex].args[1] = (long)buf;
    btable[off + curindex].args[2] = count;
    btable[off + curindex].pid = main_thread_pid + off;
    curindex = (curindex == table_size - 1) ? 1 : curindex + 1;

    return 0;
}

__attribute__((constructor)) static void setup(void) {
    size_t pgsize = getpagesize();
    curindex = 1;
    main_thread_pid = syscall(186);
    btable = (struct batch_entry*)aligned_alloc(pgsize, pgsize * MAX_THREAD_NUM);
    syscall(__NR_register, btable);
}