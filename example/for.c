/* This is minimal example for loop */
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <linux/batch.h>

int main(int argc, char **argv) {
    int i = 0;
    for(; i < 10; i++)
        write(1, "hihi", 4);
    batch_flush();
    return 0;
}