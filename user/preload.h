#define _GNU_SOURCE

#define MAX_TABLE_SIZE 64
#define MAX_THREAD_NUM 10

#include <dlfcn.h>
#include <inttypes.h>
#include <linux/batch.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

struct pthread_fake {
    /* offset to find tid */
    void *nothing[90];
    pid_t tid;
};

struct batch_entry *btable;
extern int curindex[MAX_THREAD_NUM];

typedef long (*open_t)(const char *pathname, int flags, mode_t mode);
open_t real_open;
typedef long (*read_t)(int fd, void *buf, size_t count);
read_t real_read;
typedef long (*write_t)(unsigned int fd, const char *buf, size_t count);
write_t real_write;
typedef long (*close_t)(int fd);
close_t real_close;
