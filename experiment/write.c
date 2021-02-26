#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include <../include/linux/batch.h>

//#define BATCH 0
//#define dBATCH 0
#define REPEAT 60
long get_elapse(struct timespec start, struct timespec end) {
    return ((long)1.0e+9 * end.tv_sec + end.tv_nsec) -
           ((long)1.0e+9 * start.tv_sec + start.tv_nsec);
}
int main(int argc , char *argv[])
{
    int i, j;

    if(argc < 2){
        printf("Not enough arguments\n");
        exit(-1);
    }
    int sys_num = atoi(argv[1]);

    struct timespec t1, t2;
    long sum = 0;
    char *buf = "abcdefghijk\n";
    #ifndef BATCH
    for(j = 0; j < REPEAT; j++)
    {
        clock_gettime(CLOCK_REALTIME, &t1);
        for(i = 0; i < sys_num; i++)
            write(1, buf, strlen(buf));
        #ifdef dBATCH
        /* batch_flush */
        syscall(183);
        #endif
        clock_gettime(CLOCK_REALTIME, &t2);
        sum += get_elapse(t1, t2);
    }
    sum /= REPEAT;
    printf("%ld\n", sum);

    #else
    long        ret[sys_num], r, fdAddr=(long)&ret[0];

	syscall_t   b[sys_num];

    for(i = 0; i < sys_num; i++)
        b[i] = scall3(write, -1, -1, 0, 1, buf, strlen(buf));

    for(j = 0; j < REPEAT; j++)
    {
        clock_gettime(CLOCK_REALTIME, &t1);
        if ((r = batch(ret, b, sys_num, 0, 0)) < sys_num - 1) { /* rval is highest done INDEX */
            //if (eno) *eno = -ret[r];
            return -1;
        }
        clock_gettime(CLOCK_REALTIME, &t2);
        sum += get_elapse(t1, t2);
    }
    sum /= REPEAT;
    printf("%ld\n", sum);
    #endif


    return 0;
}