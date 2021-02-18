#define _GNU_SOURCE

#define MAX_TABLE_SIZE 64
#include <dlfcn.h>
#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <linux/batch.h>

struct batch_entry *btable;

typedef ssize_t (*func_write)(int, const void *, size_t);
typedef ssize_t (*func_read)(int, const void *, size_t);
typedef int (*func_open)(const char *, int, mode_t);
typedef int (*func_close)(int);

func_write real_write = NULL;
func_read real_read = NULL;
func_open real_open = NULL;
func_close real_close = NULL;