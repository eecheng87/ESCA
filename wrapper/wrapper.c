/*
 * Syscall wrapper for Effective System Call Aggregation (ESCA).
 *
 * Copyright (c) 2021-2022 National Cheng Kung University, Taiwan.
 * Authored by Steven Cheng <yucheng871011@gmail.com>
 */

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <fcntl.h>

#include "preload.h"

int table_size = 64;
int in_segment;
void *mpool; /* memory pool */
int pool_offset;
struct iovec *iovpool; /* pool for iovector */
int iov_offset;
int batch_num; /* number of busy entry */
struct batch_entry *btable;
int global_i, global_j;
size_t pgsize;

long esca_init()
{
    btable = aligned_alloc(pgsize, pgsize * MAX_THREAD_NUM);
    syscall(__NR_register, btable);
    return 0;
}

long batch_start(int exp)
{
    in_segment = 1;
    batch_num = 0;

    return 0;
}

long batch_flush()
{
    in_segment = 0;

    /* avoid useless batch_flush */
    if (batch_num == 0)
        return 0;
    batch_num = 0;
    return syscall(__NR_batch_flush);
}

#include "preload.c"

__attribute__((constructor)) static void setup(void)
{
    pgsize = getpagesize();
    in_segment = 0;
    batch_num = 0;

    /* store glibc function */
    real_writev = real_writev ? real_writev : dlsym(RTLD_NEXT, "writev");
    real_shutdown =
        real_shutdown ? real_shutdown : dlsym(RTLD_NEXT, "shutdown");
    real_sendfile =
        real_sendfile ? real_sendfile : dlsym(RTLD_NEXT, "sendfile");

    global_i = global_j = 0;
}
