#include "preload.h"
//int curindex[MAX_THREAD_NUM];
int curindex = 1;
int table_size = 64;
int main_thread_pid = 0;
int in_segment;
void* mpool; /* memory pool */
int pool_offset;
int batch_num; /* number of busy entry */

int off, toff;
struct batch_entry* btable;
long batch_start(int events)
{
    in_segment = 1;
    if (!btable) {
        int pgsize = getpagesize();
        btable = (struct batch_entry*)aligned_alloc(pgsize, pgsize);
        //toff = (((struct pthread_fake *)pthread_self())->tid - main_thread_pid);
        toff = syscall(39) - main_thread_pid;
        toff -= 1;
        printf("Register table %d\n", toff);
        off = toff << 6;
        syscall(__NR_register, btable, toff);
    }
    return 0;
}

long batch_flush()
{
    in_segment = 0;
    /* avoid useless batch_flush */
    if (batch_num == 0)
        return 0;
    batch_num = 0;
    return syscall(__NR_batch_flush, toff);
}

int close(int fd)
{
    if (!in_segment) {
        real_close = real_close ? real_close : dlsym(RTLD_NEXT, "close");
        return real_close(fd);
    }
    batch_num++;

    btable[curindex].sysnum = 3;
    btable[curindex].rstatus = BENTRY_BUSY;
    btable[curindex].nargs = 1;
    btable[curindex].args[0] = fd;
    curindex = (curindex == MAX_TABLE_SIZE - 1) ? 1 : curindex + 1;
    if (batch_num > BATCH_NUM)
        batch_flush();
    return 0;
}

#if 1
ssize_t sendfile64(int outfd, int infd, off_t* offset, size_t count)
{
    if (!in_segment) {
        real_sendfile = real_sendfile ? real_sendfile : dlsym(RTLD_NEXT, "sendfile");
        return real_sendfile(outfd, infd, offset, count);
    }
    batch_num++;

    btable[curindex].sysnum = 40;
    btable[curindex].rstatus = BENTRY_BUSY;
    btable[curindex].nargs = 4;
    btable[curindex].args[0] = outfd;
    btable[curindex].args[1] = infd;
    btable[curindex].args[2] = /*offset*/ 0;
    btable[curindex].args[3] = count;
    curindex = (curindex == MAX_TABLE_SIZE - 1) ? 1 : curindex + 1;
    if (batch_num > BATCH_NUM)
        batch_flush();
    /* assume always success */
    return count;
}
#endif

__attribute__((constructor)) static void setup(void)
{
    int i;
    size_t pgsize = getpagesize();
    in_segment = 0;
    batch_num = 0;

    /* get pid of main thread */
    main_thread_pid = syscall(39);

    //btable =
    //  (struct batch_entry *)aligned_alloc(pgsize, pgsize * MAX_THREAD_NUM);

    //syscall(__NR_register, btable);
    //signal(SIGINT, ctrl_c_hdlr);

    //for (i = 0; i < MAX_THREAD_NUM; i++)
    //  curindex[i] = 1;
}
