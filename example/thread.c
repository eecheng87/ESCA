/* This is minimal example for thread program */

#include <linux/batch.h>
#include <pthread.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#define n 3
pthread_t t[n];
void *worker(void *arg) {
    printf("In thread pid is %ld\n", syscall(186));
    write(1, "hih\n", 4);
    write(1, "hih\n", 4);
    batch_flush();
}
int main(int argc, char **argv) {
    int i;
    printf("In main pid is %ld\n", syscall(186));
    for (i = 0; i < n; i++)
        pthread_create(&t[i], NULL, worker, NULL);
    for (i = 0; i < n; i++)
        pthread_join(t[i], NULL);
    return 0;
}