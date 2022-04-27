#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <fcntl.h>
#include "preload.h"

int curindex[MAX_THREAD_NUM];
int table_size = 64;
int main_thread_pid;
int in_segment;
void *mpool; /* memory pool */
int pool_offset;
struct iovec *iovpool; /* pool for iovector */
int iov_offset;
int batch_num;   /* number of busy entry */
int syscall_num; /* number of syscall triggered currently */
struct batch_entry *btable;


long batch_start(int exp)
{
    in_segment = 1;
    batch_num = 0;
    // syscall_num = 0;
    return 0;
}

long batch_flush()
{
    in_segment = 0;

    /* avoid useless batch_flush */
    if (batch_num == 0)
        return 0;
    return syscall(__NR_batch_flush);
}

ssize_t shutdown(int fd, int how)
{
    syscall_num++;
    if (!in_segment) {
        return real_shutdown(fd, how);
    }
    batch_num++;

    int off, toff = 0, len = 0, i;
    off = curindex[1] << 6; /* 6 = log64 */

    btable[off + curindex[toff]].sysnum = 48;
    btable[off + curindex[toff]].rstatus = BENTRY_BUSY;
    btable[off + curindex[toff]].nargs = 2;
    btable[off + curindex[toff]].args[0] = fd;
    btable[off + curindex[toff]].args[1] = how;

    if (curindex[toff] == MAX_TABLE_SIZE - 1) {
        if (curindex[1] == MAX_THREAD_NUM - 1) {
            curindex[1] = 1;
        } else {
            curindex[1]++;
        }
        curindex[toff] = 1;
    } else {
        curindex[toff]++;
    }

    /* assume success */
    return 0;
}

#if 0
ssize_t sendfile64(int out_fd, int in_fd, off_t *offset, size_t count) {
    syscall_num++;
    if (!in_segment) {
        return real_sendfile(out_fd, in_fd, offset, count);
    }
    batch_num++;

    int off, toff = 0;
    off = curindex[1] << 6; /* 6 = log64 */

    btable[off + curindex[toff]].sysnum = 40;
    btable[off + curindex[toff]].rstatus = BENTRY_BUSY;
    btable[off + curindex[toff]].nargs = 4;
    btable[off + curindex[toff]].args[0] = out_fd;
    btable[off + curindex[toff]].args[1] = in_fd;
    btable[off + curindex[toff]].args[2] = offset;
    btable[off + curindex[toff]].args[3] = count;
    btable[off + curindex[toff]].pid = main_thread_pid + off;

    if (curindex[toff] == MAX_TABLE_SIZE - 1) {
        if (curindex[1] == MAX_THREAD_NUM - 1) {
            curindex[1] = 1;
        } else {
            curindex[1]++;
        }
        curindex[toff] = 1;
    } else {
        curindex[toff]++;
    }
    /* assume success */
    return count;
}

#endif
#if 1
ssize_t writev(int fd, const struct iovec *iov, int iovcnt)
{
    syscall_num++;
    if (!in_segment) {
        return real_writev(fd, iov, iovcnt);
    }
    batch_num++;

    int off, toff = 0, len = 0, i;
    off = curindex[1] << 6; /* 6 = log64 */

    for (i = 0; i < iovcnt; i++) {
        int ll = iov[i].iov_len;
        len += iov[i].iov_len;
    }

    btable[off + curindex[toff]].sysnum = 20;
    btable[off + curindex[toff]].rstatus = BENTRY_BUSY;
    btable[off + curindex[toff]].nargs = 3;
    btable[off + curindex[toff]].args[0] = fd;
    btable[off + curindex[toff]].args[1] = (long) (iov /*iovpool[iov_offset]*/);
    btable[off + curindex[toff]].args[2] = iovcnt;
    btable[off + curindex[toff]].pid = main_thread_pid + off;

    if (curindex[toff] == MAX_TABLE_SIZE - 1) {
        if (curindex[1] == MAX_THREAD_NUM - 1) {
            curindex[1] = 1;
        } else {
            curindex[1]++;
        }
        curindex[toff] = 1;
    } else {
        curindex[toff]++;
    }
    /* assume always success */
    return len;
}
#endif



__attribute__((constructor)) static void setup(void)
{
    int i;
    size_t pgsize = getpagesize();
    in_segment = 0;
    batch_num = 0;
    syscall_num = 0;

    /* get pid of main thread */
    main_thread_pid = syscall(39);
    btable =
        (struct batch_entry *) aligned_alloc(pgsize, pgsize * MAX_THREAD_NUM);

    /* store glibc function */
    real_writev = real_writev ? real_writev : dlsym(RTLD_NEXT, "writev");
    real_shutdown =
        real_shutdown ? real_shutdown : dlsym(RTLD_NEXT, "shutdown");
    real_sendfile =
        real_sendfile ? real_sendfile : dlsym(RTLD_NEXT, "sendfile");

    syscall(__NR_register, btable);
    for (i = 0; i < MAX_THREAD_NUM; i++)
        curindex[i] = 1;
}
