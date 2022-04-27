#pragma once

#define _GNU_SOURCE

#define MAX_TABLE_SIZE 64
#define MAX_THREAD_NUM 10
#define MAX_POOL_SIZE 130172
#define POOL_UNIT 8
#define BATCH_NUM 50

#include <dlfcn.h>
#include <inttypes.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/uio.h>

#include "../include/linux/esca.h"

struct pthread_fake {
    /* offset to find tid */
    void *nothing[90];
    pid_t tid;
};

extern struct batch_entry *btable;

typedef long (*close_t)(int fd);
close_t real_close;
typedef long (*sendfile_t)(int outfd, int infd, off_t *offset, size_t count);
sendfile_t real_sendfile;
typedef long (*shutdown_t)(int fd, int how);
shutdown_t real_shutdown;
typedef long (*writev_t)(int fd, const struct iovec *iov, int iovcnt);
writev_t real_writev;
