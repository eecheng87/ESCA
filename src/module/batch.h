#ifndef LINUX_BATCH_H /* define syscall_t batch(), batcha?() for user space */
#define LINUX_BATCH_H /* and syscall_t __NR_batch for the kernel module. */
/*
  This structure encapsulates what sys_batch needs to know to make an indirect
  system call.  It constitutes a very simple micro-language with system calls as
  atoms.  There are just 3 very common in-kernel syscall circumstances: failure,
  success with no pending work, and success with pending work or else An Answer.
  In-kernel, these circumstances are basically always ret < 0, == 0 > 0.  For
  each circumstance, the structure allows encoding a conditional forward jump
  in the passed syscall_t[] slots.  Backward jumps aka loops are disallowed due
  to halting concerns.  Any out of bounds jump terminates the batch call.

  sys_batch adds two fake system calls wdcpy & jmpfwd - inlined in the dispatch
  loop.  wdcpy takes two user space addresses and copies a register-sized word.
  Combined with forward jumps, this can be used to "chain" bundles of syscalls
  together, e.g., appropriate combinations of (open, fstat, wdcpy, mmap, close)
  and wdcpy's to mmap an whole file in one user-kernel crossing.  Arithmetic &
  boolean logic is not yet supported (but could be added as new fake syscalls).
*/
#include <asm/unistd.h>                 /* __NR_* */
#define __NR_batch  __NR_afs_syscall    /* Hijack Andrew FS call slot for now */
#define __NR_wdcpy  -2                  /* word copy: resolved in sys_batch */
#define __NR_jmpfwd -3                  /* jump forward: resolved in sys_batch */
#define __NR_assign -4
#define __NR_strcmp -5

typedef struct syscall {
	short nr,                       /* syscall number */
	      jumpFail,                 /* -4096 < in-kern return < 0 jump */
	      jump0,                    /* == 0 in-kernel return jump */
	      jumpPos;                  /* > 0 in-kernel return jump */
	char  argc;                     /* argument count. XXX argc[nr] */
	long  arg[6];                   /* args for this call */
} syscall_t;

#ifndef __KERNEL__
#include <errno.h>                      /* needed by syscall macro */
#ifndef syscall
#   include <unistd.h>                  /* syscall() */
#endif

static inline long indirect_call(int nr, int argc, long *a) {
	switch (argc) {
		case 0: return syscall(nr);
		case 1: return syscall(nr, a[0]);
		case 2: return syscall(nr, a[0], a[1]);
		case 3: return syscall(nr, a[0], a[1], a[2]);
		case 4: return syscall(nr, a[0], a[1], a[2], a[3]);
		case 5: return syscall(nr, a[0], a[1], a[2], a[3], a[4]);
		case 6: return syscall(nr, a[0], a[1], a[2], a[3], a[4], a[5]);
	}
	return ENOSYS;
}

static inline long batchE(long rets[], struct syscall calls[],
                          unsigned long ncall, long size, long flags)
{
	syscall_t *c;
	unsigned   i;
	long       r = 0, off = 0;
	if (ncall == 0)
		return 0;
	for (i = 0; i < ncall; i += 1 + off) {
		c = &calls[i];
		if (c->nr == __NR_wdcpy) {
			*(long *)(c->arg[0]) = *(long *)(c->arg[1]);
			continue;
		} else if (c->nr == __NR_jmpfwd) {
			off = c->jumpFail;
			goto cont;
		}                       // |-- __NR_syscall_max only in KERNEL
		if (c->nr < 0 || c->nr > 1024 || c->argc > 6) {
			rets[i] = -ENOSYS;
			return i;
		}
		r = indirect_call(c->nr, c->argc, c->arg);
		if ((unsigned long)r > 18446744073709547520ULL) { // -4096 < r < 0
			off = c->jumpFail;
			rets[i] = -errno;
		} else if (r > 0) {
			off = c->jumpPos;
			rets[i] = r;
		} else {
			off = c->jump0;
			rets[i] = 0;
		}
cont:		if (off < 0)
			return i;
	}
	return ncall - 1;
}

static int batch_emul_init(void) {      // Auto-detect unless $BATCH_EMUL..
	char *getenv(const char *);     //..forces pure user-space emulation.
	return !!getenv("BATCH_EMUL") ||
	       (syscall(__NR_batch, (long *)0, (syscall_t *)0, 0, 0, 0) != 0);
}

static inline long batch(long rets[], struct syscall calls[],
                         unsigned long ncall, long flags, long size)
{
	static int batch_emul = -1;
	if (batch_emul == -1)
		batch_emul = batch_emul_init();
	return batch_emul ? batchE(rets, calls, ncall, flags, size) :
	                   syscall(__NR_batch, rets, calls, ncall, flags, size);
}

#define scall0(N,FL,Z,P            ) ((syscall_t){__NR_##N,FL,Z,P,0 })
#define scall1(N,FL,Z,P,A          ) ((syscall_t){__NR_##N,FL,Z,P,1,(long)(A) })
#define scall2(N,FL,Z,P,A,B        ) ((syscall_t){__NR_##N,FL,Z,P,2,(long)(A),(long)(B) })
#define scall3(N,FL,Z,P,A,B,C      ) ((syscall_t){__NR_##N,FL,Z,P,3,(long)(A),(long)(B),(long)(C) })
#define scall4(N,FL,Z,P,A,B,C,D    ) ((syscall_t){__NR_##N,FL,Z,P,4,(long)(A),(long)(B),(long)(C),(long)(D) })
#define scall5(N,FL,Z,P,A,B,C,D,E  ) ((syscall_t){__NR_##N,FL,Z,P,5,(long)(A),(long)(B),(long)(C),(long)(D),(long)(E) })
#define scall6(N,FL,Z,P,A,B,C,D,E,F) ((syscall_t){__NR_##N,FL,Z,P,6,(long)(A),(long)(B),(long)(C),(long)(D),(long)(E),(long)(F)})
#endif /* __KERNEL__ */
#endif /* LINUX_BATCH_H */
