#define _GNU_SOURCE

#define MAX_TABLE_SIZE 64
#include <dlfcn.h>
#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <linux/batch.h>

struct batch_entry *btable;
extern int baseindex;
extern int curindex;