#include "preload.h"
int curindex[MAX_THREAD_NUM];
int table_size = 64;
int main_thread_pid;

int open(const char *pathname, int flags, mode_t mode) {
    int off,
        toff = (((struct pthread_fake *)pthread_self())->tid - main_thread_pid);
    off = toff << 6; /* 6 = log64 */
    btable[off + curindex[toff]].sysnum = __NR_open;
    btable[off + curindex[toff]].rstatus = BENTRY_BUSY;
    btable[off + curindex[toff]].nargs = 3;
    btable[off + curindex[toff]].args[0] = (long)pathname;
    btable[off + curindex[toff]].args[1] = flags;
    btable[off + curindex[toff]].args[2] = mode;
    btable[off + curindex[toff]].pid = main_thread_pid + off;
    curindex[toff] =
        (curindex[toff] == MAX_TABLE_SIZE - 1) ? 1 : curindex[toff] + 1;

    /* memorize the -index of fd */
    return -(curindex[toff] - 1);
}

int close(int fd) {
    int off,
        toff = (((struct pthread_fake *)pthread_self())->tid - main_thread_pid);
    off = toff << 6; /* 6 = log64 */
    btable[off + curindex[toff]].sysnum = __NR_close;
    btable[off + curindex[toff]].rstatus = BENTRY_BUSY;
    btable[off + curindex[toff]].nargs = 1;
    btable[off + curindex[toff]].args[0] = fd;
    btable[off + curindex[toff]].pid = main_thread_pid + off;
    curindex[toff] =
        (curindex[toff] == MAX_TABLE_SIZE - 1) ? 1 : curindex[toff] + 1;

    return 0;
}

ssize_t write(int fd, const void *buf, size_t count) {
    int off,
        toff = (((struct pthread_fake *)pthread_self())->tid - main_thread_pid);
    off = toff << 6; /* 6 = log64 */
    btable[off + curindex[toff]].sysnum = __NR_write;
    btable[off + curindex[toff]].rstatus = BENTRY_BUSY;
    btable[off + curindex[toff]].nargs = 3;
    btable[off + curindex[toff]].args[0] = fd;
    btable[off + curindex[toff]].args[1] = (long)buf;
    btable[off + curindex[toff]].args[2] = count;
    btable[off + curindex[toff]].pid = main_thread_pid + off;

    curindex[toff] =
        (curindex[toff] == MAX_TABLE_SIZE - 1) ? 1 : curindex[toff] + 1;

    return 0;
}

ssize_t read(int fd, void *buf, size_t count) {
    int off,
        toff = (((struct pthread_fake *)pthread_self())->tid - main_thread_pid);
    off = toff << 6; /* 6 = log64 */
    btable[off + curindex[toff]].sysnum = __NR_read;
    btable[off + curindex[toff]].rstatus = BENTRY_BUSY;
    btable[off + curindex[toff]].nargs = 3;
    btable[off + curindex[toff]].args[0] = fd;
    btable[off + curindex[toff]].args[1] = (long)buf;
    btable[off + curindex[toff]].args[2] = count;
    btable[off + curindex[toff]].pid = main_thread_pid + off;
    curindex[toff] =
        (curindex[toff] == MAX_TABLE_SIZE - 1) ? 1 : curindex[toff] + 1;

    return 0;
}

__attribute__((constructor)) static void setup(void) {
    int i;
    size_t pgsize = getpagesize();

    /* get pid of main thread */
    main_thread_pid = syscall(186);
    btable =
        (struct batch_entry *)aligned_alloc(pgsize, pgsize * MAX_THREAD_NUM);
    syscall(__NR_register, btable);
    for (i = 0; i < MAX_THREAD_NUM; i++)
        curindex[i] = 1;
}