/*
 * Linux kernel module for Effective System Call Aggregation (ESCA).
 *
 * Copyright (c) 2021-2022 National Cheng Kung University, Taiwan.
 * Authored by Steven Cheng <yucheng871011@gmail.com>
 */

#include <generated/asm-offsets.h> /* __NR_syscall_max */
#include <linux/batch.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/kallsyms.h> /* kallsyms_lookup_name, __NR_* */
#include <linux/kdev_t.h>
#include <linux/kernel.h> /* Basic Linux module headers */
#include <linux/module.h>
#include <linux/pagemap.h>
#include <linux/sched/signal.h>
#include <linux/slab.h>
#include <linux/uaccess.h> /* copy_from_user put_user */
#include <linux/version.h>
#include <linux/vmalloc.h>
#include "scTab.h"

MODULE_DESCRIPTION("Generic batch system call API");
MODULE_LICENSE("Dual MIT/GPL");
MODULE_AUTHOR("National Cheng Kung University, Taiwan");
MODULE_VERSION("0.1");

struct page *pinned_pages[MAX_THREAD_NUM];
static void **scTab = 0;

typedef asmlinkage long (*F0_t)(void);
typedef asmlinkage long (*F1_t)(long);
typedef asmlinkage long (*F2_t)(long, long);
typedef asmlinkage long (*F3_t)(long, long, long);
typedef asmlinkage long (*F4_t)(long, long, long, long);
typedef asmlinkage long (*F5_t)(long, long, long, long, long);
typedef asmlinkage long (*F6_t)(long, long, long, long, long, long);

static inline long indirect_call(void *f, int argc, long *a)
{ /* x64 syscall calling convention changed @4.17 to use
     struct pt_regs */
    struct pt_regs regs;
    memset(&regs, 0, sizeof regs);
    switch (argc) {
    case 6:
        regs.r9 = a[5]; /* Falls through. */
    case 5:
        regs.r8 = a[4]; /* Falls through. */
    case 4:
        regs.r10 = a[3]; /* Falls through. */
    case 3:
        regs.dx = a[2]; /* Falls through. */
    case 2:
        regs.si = a[1]; /* Falls through. */
    case 1:
        regs.di = a[0]; /* Falls through. */
    }
    return ((F1_t) f)((long) &regs);
}

#if defined(__x86_64__)
extern unsigned long __force_order __weak;
#define store_cr0(x) asm volatile("mov %0,%%cr0" : "+r"(x), "+m"(__force_order))
static void allow_writes(void)
{
    unsigned long cr0 = read_cr0();
    clear_bit(16, &cr0);
    store_cr0(cr0);
}
static void disallow_writes(void)
{
    unsigned long cr0 = read_cr0();
    set_bit(16, &cr0);
    store_cr0(cr0);
}
#elif defined(__aarch64__)
// FIXME: port to ARM64
#endif

struct batch_entry *batch_table[MAX_THREAD_NUM];
int table_size = 64;
int start_index[MAX_THREAD_NUM];
int global_i, global_j;
int main_pid; /* PID of main thread */

asmlinkage long sys_register(const struct pt_regs *regs)
{
    int n_page, i, j;
    unsigned long p1 = regs->di;

    /* map batch table from user-space to kernel */
    n_page = get_user_pages(
        (p1),           /* Start address to map */
        MAX_THREAD_NUM, /* Number of pinned pages. 4096 btyes in this machine */
        FOLL_FORCE | FOLL_WRITE, /* Force flag */
        pinned_pages,            /* struct page ** pointer to pinned pages */
        NULL);

    for (i = 0; i < MAX_THREAD_NUM; i++)
        batch_table[i] = (struct batch_entry *) kmap(pinned_pages[i]);

    /* initial table status */
    for (j = 0; j < MAX_THREAD_NUM; j++)
        for (i = 0; i < MAX_ENTRY_NUM; i++)
            batch_table[j][i].rstatus = BENTRY_EMPTY;

    global_i = global_j = 0;

    main_pid = current->pid;

    return 0;
}


asmlinkage long sys_batch(void)
{
    int j = global_j, i = global_i, cnt = 0;

#if DEBUG
    printk(KERN_INFO "Start flushing, started from index: %d\n", i);
#endif
    while (batch_table[j][i].rstatus == BENTRY_BUSY) {
#if DEBUG
        cnt++;
        printk(KERN_INFO "Index %d do syscall %d (%d %d)\n", i,
               batch_table[j][i].sysnum, j, i);
#endif
        batch_table[j][i].sysret =
            indirect_call(scTab[batch_table[j][i].sysnum],
                          batch_table[j][i].nargs, batch_table[j][i].args);
        batch_table[j][i].rstatus = BENTRY_EMPTY;

        if (i == MAX_ENTRY_NUM - 1) {
            if (j == MAX_THREAD_NUM - 1) {
                j = 0;
            } else {
                j++;
            }
            i = 0;
        } else {
            i++;
        }
    }
#if DEBUG
    printk(KERN_INFO "batch %d syscalls\n", cnt);
#endif
    global_i = i;
    global_j = j;
    return 0;
}

void *sys_oldcall0;
void *sys_oldcall1;

static int __init mod_init(void)
{
    int rc;
    scTab = (void **) (smSCTab + ((char *) &system_wq - smSysWQ));
    allow_writes();

    /* backup */
    sys_oldcall0 = scTab[__NR_batch_flush];
    sys_oldcall1 = scTab[__NR_register];

    /* hooking */
    scTab[__NR_batch_flush] = sys_batch;
    scTab[__NR_register] = sys_register;

    disallow_writes();

    printk(KERN_INFO "batch: installed as %d\n", __NR_batch_flush);
    return 0;
}
static void __exit mod_cleanup(void)
{
    printk(KERN_INFO "batch: removed\n");
    allow_writes();

    /* restore */
    scTab[__NR_batch_flush] = sys_oldcall0;
    scTab[__NR_register] = sys_oldcall1;

    disallow_writes();
    /* correspond cleanup for kmap */
    kunmap(pinned_pages[0]);
}
module_init(mod_init);
module_exit(mod_cleanup);
