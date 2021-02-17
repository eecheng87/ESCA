#include <linux/kernel.h>               /* Basic Linux module headers */
#include <linux/module.h>
#include <linux/kallsyms.h>             /* kallsyms_lookup_name, __NR_* */
#include <generated/asm-offsets.h>      /* __NR_syscall_max */
#include "batch.h"                      /* struct syscall, __NR_batch */
#include <linux/uaccess.h>              /* copy_from_user put_user */
#include <linux/mm.h>
#include <linux/version.h>
#include <linux/slab.h>
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,12,0)
#	include <linux/signal.h>
#else
#	include <linux/sched/signal.h>
#endif

MODULE_DESCRIPTION("Generic batch system call API");
MODULE_AUTHOR("Charles Blake <charles.l.blake2@gmail.com>");
MODULE_LICENSE("GPL v2");

static inline long segv(void const *Addr)
{
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,12,0)
	struct siginfo info;
#else
	struct kernel_siginfo info;
#endif
	memset(&info, 0, sizeof info);
	info.si_signo = SIGSEGV;
	info.si_addr = (void *)Addr;
	return send_sig_info(SIGSEGV, &info, current);
}

typedef asmlinkage long (*F0_t)(void);
typedef asmlinkage long (*F1_t)(long);
typedef asmlinkage long (*F2_t)(long, long);
typedef asmlinkage long (*F3_t)(long, long, long);
typedef asmlinkage long (*F4_t)(long, long, long, long);
typedef asmlinkage long (*F5_t)(long, long, long, long, long);
typedef asmlinkage long (*F6_t)(long, long, long, long, long, long);

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,17,0)
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
#else
static inline long indirect_call(void *f, int argc, long *a)
{
	switch (argc) {
		case 0: return ((F0_t)f)();
		case 1: return ((F1_t)f)(a[0]);
		case 2: return ((F2_t)f)(a[0], a[1]);
		case 3: return ((F3_t)f)(a[0], a[1], a[2]);
		case 4: return ((F4_t)f)(a[0], a[1], a[2], a[3]);
		case 5: return ((F5_t)f)(a[0], a[1], a[2], a[3], a[4]);
		case 6: return ((F6_t)f)(a[0], a[1], a[2], a[3], a[4], a[5]);
	}
	return -ENOSYS;
}
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,7,0)
#include "scTab.h"
#endif
static void **scTab = 0;
static char block[1024] = { 0 }; /*XXX allow whitelist not block blacklist? */

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,17,0)
asmlinkage long sys_batch(const struct pt_regs *regs)
{
	unsigned long ur0   = regs->di;
	unsigned long uc0   = regs->si;
	unsigned long ncall = regs->dx;
//	unsigned long flags = regs->r10;
//	unsigned long size  = regs->r8;
#else
asmlinkage long sys_batch(unsigned long ur0, unsigned long uc0, unsigned long ncall,
        __attribute__((unused)) long flags, __attribute__((unused)) long size)
{
#endif
	long          r = 0, off = 0, *krv = NULL;
	unsigned long i = 0, urEnd = ur0 + ncall * sizeof(long),
	              aEnd = uc0 + ncall * sizeof(syscall_t), kr0, kc0;
	syscall_t    *calls = (syscall_t *)uc0, *buf = NULL;
	gfp_t         gfp_flags = GFP_KERNEL | ___GFP_ZERO;
        if (ncall == 0)
            return 0;
	if (!(buf = kmalloc(ncall * (sizeof *krv + sizeof *calls), gfp_flags)))
		return -ENOMEM;
	krv = (long *)(kr0 = (unsigned long)buf);
	kc0 = kr0 + ncall * sizeof(long);
	if (unlikely(copy_from_user((void *)kc0, calls, ncall * sizeof *calls)))
		{ krv[i] = -EFAULT; goto errRet; }
	for (/**/; i < ncall; i += 1 + off) {
		void      *f;
		syscall_t *c = (syscall_t *)(kc0 + i * sizeof *c);
		if (c->nr == __NR_wdcpy) {
			unsigned long tmp;               // Do *dst = *src
			unsigned long src = (unsigned long)c->arg[1];
			unsigned long dst = (unsigned long)c->arg[0];
			if (ur0 <= src && src < urEnd)
				tmp = *(unsigned long *)(kr0 + (src - ur0));
			else if (unlikely(get_user(tmp, (long *)c->arg[1])))
				{ krv[i] = -EFAULT; goto errRet; }
			if (uc0 <= dst && dst < aEnd)
				*(unsigned long *)(kc0 + (dst - uc0)) = tmp;
			else if (unlikely(put_user(tmp, (long *)c->arg[0])))
				{ krv[i] = -EFAULT; goto errRet; }
			continue;
		} else if (c->nr == __NR_jmpfwd) {
			off = c->jumpFail;
			goto cont;
		}
		if (unlikely(c->nr < 0 || c->nr > __NR_syscall_max ||
		             block[c->nr] || c->argc > 6 || !(f = scTab[c->nr])))
			{ krv[i] = -ENOSYS; goto errRet; }
		r = krv[i] = indirect_call(f, c->argc, c->arg);
		if ((unsigned long)r > 18446744073709547520ULL) // -4096 < r < 0
			off = c->jumpFail;
		else if (r > 0)
			off = c->jumpPos;
		else
			off = c->jump0;
cont:		if (off < 0)
			break;
	}
	if (i >= ncall)
		i--;
errRet: if (unlikely(copy_to_user((void *)ur0, krv, (i + 1) * sizeof(long))))
		segv((void *)ur0); // Cannot store EFAULT
	kfree(buf);
	return i;
}
extern unsigned long __force_order __weak;
#define store_cr0(x) asm volatile("mov %0,%%cr0" : "+r"(x), "+m"(__force_order))
static void allow_writes(void) {
	unsigned long cr0 = read_cr0(); clear_bit(16, &cr0); store_cr0(cr0);
}
static void disallow_writes(void) {
	unsigned long cr0 = read_cr0(); set_bit(16, &cr0); store_cr0(cr0);
}
void *sys_oldcall0;         /* Likely sys_ni_syscall, but save/restore anyway */
static int __init mod_init(void) {
#if LINUX_VERSION_CODE < KERNEL_VERSION(5,7,0)
	if (!(scTab = (void **)kallsyms_lookup_name("sys_call_table"))) {
		printk(KERN_ERR "batch: cannot find sys_call_table; "
		       "Need CONFIG_KALLSYMS & CONFIG_KALLSYMS_ALL\n");
		return -ENOSYS;
	}
#else
	scTab = (void **)(smSCTab + ((char *)&system_wq - smSysWQ));
#endif
	sys_oldcall0 = scTab[__NR_batch];          /* save syscall */
	memset(&block[0], 0, sizeof block);        /* maybe redundant. */
	block[__NR_batch] = block[__NR_execve] = block[__NR_vfork] /*XXX*/ = 1;
	allow_writes();
	scTab[__NR_batch] = sys_batch;             /* install sys_batch */
	disallow_writes();
	printk(KERN_INFO "batch: installed as %d\n", __NR_batch);
	return 0;
}
static void __exit mod_cleanup(void) {
	allow_writes();
	scTab[__NR_batch] = sys_oldcall0;          /* restore syscall */
	disallow_writes();
	printk(KERN_INFO "batch: removed\n");
}
module_init(mod_init);
module_exit(mod_cleanup);
