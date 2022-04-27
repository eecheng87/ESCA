# Effective System Call Aggregation (ESCA)

The main objective of this work is to reduce the per-syscall overhead through
an advanced form of syscall aggregation, which is more flexible and applicable
to service-oriented scenarios, offering full compatibility with the existing
OS kernels and syscall interfaces.

This branch illustrates how Nginx and lighttpd benefit from ESCA.

## Prerequisite
For Nginx and wrk:
```shell
sudo apt install build-essential libpcre3 libpcre3-dev zlib1g zlib1g-dev
sudo apt install libssl-dev libgd-dev libxml2 libxml2-dev uuid-dev
sudo apt install autoconf automake libtool
```

## Download project
```shell
git clone https://github.com/eecheng87/ESCA
cd ESCA
```

## Config demo target
Choose either Nginx:
```shell
make config TARGET=nginx
```

or lighttpd:
```shell
make config TARGET=lighttpd
```

## Build ESCA
Compile files under `/module` and `/wrapper` (This step must be taken before next step)
```shell
make
```

## Build experimental target
Build `wrk`
```shell
make wrk
```

Download & configure nginx
```shell
make nginx
```

Download & build lighttpd
```shell
make lighttpd
```

## Testing

### Launch Nginx
Choose either
```shell
make nginx-launch # origin nginx
```
or

```shell
make load-lkm
make nginx-esca-launch # nginx-esca
```

### Launch lighttpd
Choose either
```shell
make lighttpd-launch # origin lighttpd
```
or

```shell
make load-lkm
make lighttpd-esca-launch # lighttpd-esca
```

### Download workloads
```shell
git submodule init
git submodule update
```

### Benchmarking
```shell
# nginx is at port 8081; lighttpd is at port 3000
downloads/wrk-master/wrk -c 50 -d 5s -t 4 http://localhost:8081/a20.html
```

### Demo
![image](assets/demo.gif)

Nginx-ESCA shows 11% improvements over vanilla Nginx.

![image](assets/light-demo.gif)

lighttpd-ESCA shows 13% improvements over vanilla lighttpd.

### Referencing our work
ESCA: Effective System Call Aggregation for Event-Driven Servers

*Yu-Cheng Cheng, Ching-Chun (Jim) Huang, Chia-Heng Tu*

PDP 2022: [paper](https://ieeexplore.ieee.org/abstract/document/9756707)
```
@INPROCEEDINGS{9756707,
    author={Cheng, Yu-Cheng and Huang, Ching-Chun (Jim) and Tu, Chia-Heng},
    booktitle={2022 30th Euromicro International Conference on Parallel, Distributed and Network-based Processing (PDP)},
    title={ESCA: Effective System Call Aggregation for Event-Driven Servers},
    year={2022},
    pages={18-25},
    doi={10.1109/PDP55904.2022.00012}
}
```
