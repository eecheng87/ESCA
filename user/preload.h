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
