#include <generated/asm-offsets.h> /* __NR_syscall_max */
#include <linux/file.h>
#include <linux/fs.h>
#include <linux/kallsyms.h> /* kallsyms_lookup_name, __NR_* */
#include <linux/kernel.h>   /* Basic Linux module headers */
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/pagemap.h>
#include <linux/sched/signal.h>
#include <linux/slab.h>
#include <linux/uaccess.h> /* copy_from_user put_user */
#include <linux/version.h>
#include "../include/linux/batch.h"

#include "scTab.h"

MODULE_DESCRIPTION("Generic batch system call API");
MODULE_AUTHOR("Steven Cheng");
MODULE_LICENSE("GPL v2");

struct page *pinned_pages[MAX_THREAD_NUM];
struct page *pinned_pages2[MAX_THREAD_NUM];

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

static void **scTab = 0;
struct batch_entry *batch_table[MAX_THREAD_NUM];
struct batch_entry *b1[MAX_THREAD_NUM];
struct batch_entry *b2[MAX_THREAD_NUM];

int start_index[MAX_THREAD_NUM];
int main_pid; /* PID of main thread */

asmlinkage long sys_register(const struct pt_regs *regs)
{
    int n_page, i;
    unsigned long p1 = regs->di;
    long wkr = regs->si;

    printk(KERN_INFO "Start register, address at regs is %p\n", regs);

    /* map batch table from user-space to kernel */
    n_page = get_user_pages(
        (unsigned long) (p1),    /* Start address to map */
        /*MAX_THREAD_NUM*/ 1,    /* Number of pinned pages. 4096 btyes in this
                                    machine */
        FOLL_FORCE | FOLL_WRITE, /* Force flag */
        &pinned_pages[wkr],      /* struct page ** pointer to pinned pages */
        NULL);

    batch_table[wkr] = (struct batch_entry *) kmap(pinned_pages[wkr]);
    for (i = 0; i < MAX_ENTRY_NUM; i++)
        batch_table[wkr][i].rstatus = BENTRY_EMPTY;

    start_index[wkr] = 1;

    return 0;
}

/* printk is only for debug usage */
/* it will lower a lot performance */
int infd = -1;
asmlinkage long sys_batch(const struct pt_regs *regs)
{
    int j = regs->di;
    unsigned long i = start_index[j];
#if DEBUG
    printk(KERN_INFO "Start flushing (at [%d][%lu]), called from %d\n", j, i,
           j);
#endif
    while (batch_table[j][i].rstatus == BENTRY_BUSY) {
        batch_table[j][i].sysret =
            indirect_call(scTab[batch_table[j][i].sysnum],
                          batch_table[j][i].nargs, batch_table[j][i].args);
        batch_table[j][i].rstatus = BENTRY_EMPTY;

#if DEBUG
        printk(KERN_INFO "syscall(%d, %ld,%ld,%ld,%ld);ret = %d\n",
               batch_table[j][i].sysnum, batch_table[j][i].args[0],
               batch_table[j][i].args[1], batch_table[j][i].args[2],
               batch_table[j][i].args[3], batch_table[j][i].sysret);
#endif
        i = (i == 63) ? 1 : i + 1;
    }
    start_index[j] = i;
    return 0;
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
// skip
#endif

void *sys_oldcall0;
void *sys_oldcall1;
void *syscall_emp_ori;
void *syscall_emp_ori2;

static int __init mod_init(void)
{
    /* hook system call */
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
    allow_writes();

    /* restore */
    scTab[__NR_batch_flush] = sys_oldcall0;
    scTab[__NR_register] = sys_oldcall1;

    disallow_writes();
    printk(KERN_INFO "batch: removed\n");

    /* correspond cleanup for kmap */
    kunmap(pinned_pages[0]);
}
module_init(mod_init);
module_exit(mod_cleanup);
