#include <linux/kernel.h>               /* Basic Linux module headers */
#include <linux/module.h>
#include <linux/kallsyms.h>             /* kallsyms_lookup_name, __NR_* */
#include <generated/asm-offsets.h>      /* __NR_syscall_max */
#include <linux/uaccess.h>              /* copy_from_user put_user */
#include <linux/mm.h>
#include <linux/version.h>
#include <linux/slab.h>
#include <linux/sched/signal.h>
#include <linux/pagemap.h>
#include <linux/batch.h>


#include "scTab.h"

MODULE_DESCRIPTION("Generic batch system call API");
MODULE_AUTHOR("Steven Cheng");
MODULE_LICENSE("MIT License");

struct page *pinned_pages[1];

typedef asmlinkage long (*F0_t)(void);
typedef asmlinkage long (*F1_t)(long);
typedef asmlinkage long (*F2_t)(long, long);
typedef asmlinkage long (*F3_t)(long, long, long);
typedef asmlinkage long (*F4_t)(long, long, long, long);
typedef asmlinkage long (*F5_t)(long, long, long, long, long);
typedef asmlinkage long (*F6_t)(long, long, long, long, long, long);


static inline long indirect_call(void *f, int argc, long *a)
{	/* x64 syscall calling convention changed @4.17 to use struct pt_regs */
	struct pt_regs regs;
	memset(&regs, 0, sizeof regs);
	switch (argc) {
		case 6: regs.r9  = a[5]; /* Falls through. */
		case 5: regs.r8  = a[4]; /* Falls through. */
		case 4: regs.r10 = a[3]; /* Falls through. */
		case 3: regs.dx  = a[2]; /* Falls through. */
		case 2: regs.si  = a[1]; /* Falls through. */
		case 1: regs.di  = a[0]; /* Falls through. */
	}
	return ((F1_t)f)((long)&regs);
}


static void **scTab = 0;
struct batch_entry *batch_table;
int table_size = 64;
int start_index;

asmlinkage long sys_register(const struct pt_regs *regs)
{
    printk(KERN_INFO "Start register, address at regs is %p\n", regs);
    int n_page, i;
    unsigned long p1 = regs->di;

    /* map batch table from user-space to kernel */
    n_page = get_user_pages(
    (unsigned long)(p1), /* Start address to map */
    1, /* Number of pinned pages. 4096 btyes in this machine */
    FOLL_FORCE | FOLL_WRITE, /* Force flag */
    pinned_pages,            /* struct page ** pointer to pinned pages */
    NULL);

    batch_table = (struct batch_entry*)kmap(pinned_pages[0]);

    /* initial table status */
    for(i = 0; i < table_size; i++){
        batch_table[i].rstatus = BENTRY_EMPTY;
    }

    start_index = 1;

    return 0;
}

asmlinkage long sys_batch(const struct pt_regs *regs)
{
	unsigned long start   = regs->di;
	unsigned long end   = regs->si;
	//unsigned long ncall = regs->dx;
    printk(KERN_INFO "Start flushing\n");
    unsigned long i = start_index, ret;
    while(batch_table[i].rstatus == BENTRY_BUSY){
        printk(KERN_INFO "Index %ld do syscall %d\n", i, batch_table[i].sysnum);
        switch (batch_table[i].sysnum)
        {
        case __NR_write:
        case __NR_read:
        case __NR_close:
        {
            int fd = batch_table[i].args[0];
            batch_table[i].args[0] = fd < 0 ? batch_table[-fd].sysret : fd;
            break;
        }
        default:
            break;
        }
        batch_table[i].sysret = indirect_call(scTab[batch_table[i].sysnum], batch_table[i].nargs, batch_table[i].args);
        batch_table[i].rstatus = BENTRY_EMPTY;
        i = (i == 63) ? 1 : i + 1;
    }
    start_index = i;
    return 0;
}

extern unsigned long __force_order __weak;
#define store_cr0(x) asm volatile("mov %0,%%cr0" : "+r"(x), "+m"(__force_order))
static void allow_writes(void) {
	unsigned long cr0 = read_cr0();
    clear_bit(16, &cr0);
    store_cr0(cr0);
}
static void disallow_writes(void) {
	unsigned long cr0 = read_cr0();
    set_bit(16, &cr0);
    store_cr0(cr0);
}

void *sys_oldcall0;
void *sys_oldcall1;

static int __init mod_init(void) {

    /* hook system call */
	scTab = (void **)(smSCTab + ((char *)&system_wq - smSysWQ));

    allow_writes();

    /* backup */
	sys_oldcall0 = scTab[__NR_batch];
    sys_oldcall1 = scTab[__NR_register];

    /* hooking */
	scTab[__NR_batch] = sys_batch;
    scTab[__NR_register] = sys_register;

	disallow_writes();

	printk(KERN_INFO "batch: installed as %d\n", __NR_batch);

	return 0;
}
static void __exit mod_cleanup(void) {
	allow_writes();

    /* restore */
	scTab[__NR_batch] = sys_oldcall0;
    scTab[__NR_register] = sys_oldcall1;

	disallow_writes();
	printk(KERN_INFO "batch: removed\n");

    /* correspond cleanup for kmap */
    kunmap(pinned_pages[0]);
}
module_init(mod_init);
module_exit(mod_cleanup);
