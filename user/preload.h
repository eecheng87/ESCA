#define _GNU_SOURCE

#define MAX_TABLE_SIZE 64
#define MAX_THREAD_NUM 10

#include <dlfcn.h>
#include <linux/batch.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

struct batch_entry *btable;
extern int curindex[MAX_THREAD_NUM];