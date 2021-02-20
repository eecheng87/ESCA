/* This is minimal example for open file & write & close */
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <linux/batch.h>

int main(int argc, char **argv) {
    char buff[8];
    int fd = open("test.txt", O_CREAT | O_WRONLY,
                   S_IWUSR | S_IRUSR | S_IRGRP | S_IWGRP | S_IROTH);
    write(fd, "hihi", 4);
    write(1, "hihi", 4);
    close(fd);
    batch_flush();
    return 0;
}