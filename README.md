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
Application is needed to be compiled with link flag `lpthread` and be executed with preload lib as prefix:
```shell
$ LD_PRELOAD=$PWD/preload.so ./test
```

More examples can be found under `/example`.

## Feature
* Support interleaved non-syscall between syscalls
* Support loop
* Support multi-thread batching (not portable, only for POSIX & linux glibc)

## Performance
So far, we only did simple experiment for measuring performance. Following is summary of experiment, we did 60 times experiment as x-axis. In each iteration, we did 60 times consecutive `write` in three methods.

![](https://i.imgur.com/YMZBOgp.png)

Although context switch is cheaper than mode transition, it still has overhead. Optimization of dBatch comes from less time to do mode transition. Mode transition has direct penalty and indirect penalty. The former is consist of executing the trap handler which copies the arguments from the registers to the kernel stack. The later is consist of TLB and cache miss.


|  | IPC | L1-dcache-load-misses | dTLB-load-misses | cache-misses | page-faults |
| -------- | -------- | -------- | -------- | -------- | -------- |
| Normal   | 0.42 | 5,8851 | 33 | 1,1330 | 55 |
| dBatch   | 2.16 | 3167 | 17 | 9471 | 48 |



Relative experiments can be found under `/experiment`

## Relative project
c-blake - [batch](https://github.com/c-blake/batch)
