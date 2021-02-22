# dBatch
This project takes the concept of c-blake's [batch](https://github.com/c-blake/batch). By decreasing the times switching between kernel and user mode, we expected to increase the performance by batching system calls.

## Build
build kernel module `batch`
```shell
$ cd module
$ sudo ./build
$ sudo insmod batch.ko
```
build pre-load library
```shell
$ cd user
$ make
```

## Usage
In dBatch, it's easier to use. The only thing need to do is add `batch_flush` which hints kernel start to batch.

Example:
```cpp
#include <linux/batch.h>

int main(int argc, char **argv) {
    int fd = open("test.txt", O_CREAT | O_WRONLY,
                   S_IWUSR | S_IRUSR | S_IRGRP | S_IWGRP | S_IROTH);
    write(fd, "hihi", 4);
    write(1, "hihi", 4);
    close(fd);
    batch_flush();
    return 0;
}
```
Application needed to be compiled with link flag `lpthread` and executed with preload lib as prefix:
```shell
$ LD_PRELOAD=$PWD/preload.so ./test
```

More examples can be found under `/example`.

## Feature


## Performance
So far, we only did simple experiment for measuring performance. Following is summary of experiment, we did 60 times experiment as x-axis. In each iteration, we did 60 times consecutive `write` in three methods.

![](https://i.imgur.com/YMZBOgp.png)

Relative experiments can be found under `/experiment`