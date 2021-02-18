#ifndef LINUX_BATCH_H /* define syscall_t batch(), batcha?() for user space */
#define LINUX_BATCH_H /* and syscall_t __NR_batch for the kernel module. */

#include <asm/unistd.h>                 /* __NR_* */
#define __NR_batch  __NR_afs_syscall    /* Hijack Andrew FS call slot for now */
#define __NR_register 184               /* Do register routine before using batch */
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

struct batch_entry {
  unsigned nargs;
  unsigned rstatus;
  unsigned sysnum;
  unsigned sysret;
  long args[6];
};

#ifndef __KERNEL__
#include <errno.h>                      /* needed by syscall macro */
#ifndef syscall
#   include <unistd.h>                  /* syscall() */
#endif

/*
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
}*/

static inline long batch_flush(int start, int end)
{
	syscall(__NR_batch, start, end);
}

static inline long batch_register(struct batch_entry* table)
{
	syscall(__NR_register, table);
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
