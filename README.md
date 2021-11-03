# How to deploy ESCA to Nginx
This branch gives code, which can simply show how nginx-esca effectively improve throughput (this version only works for x86)

## Prerequisite
For Nginx
```
sudo apt-get install build-essential libpcre3 libpcre3-dev zlib1g zlib1g-dev libssl-dev libgd-dev libxml2 libxml2-dev uuid-dev
```

For wrk
```
sudo apt-get install build-essential libssl-dev git -y
```

## Download project
```
git clone https://github.com/eecheng87/dBatch.git
cd dBatch
git checkout origin/ngx-demo
```

## Build experimental target
Build `wrk`
```
sudo make wrk
```
Download & configure nginx
```
sudo make nginx
```

## Build ESCA
Compile files under `/module` and `/wrapper`
```
sudo make
```


## Modify Nginx
1. sudo vim downloads/nginx-1.20.0/objs/Makefile
    * Remove `-Werror` in CFLAGS
    * append absolute path of `libdummy.so` (e.g. /home/wrapper/libdummy.so) to the tail of `$(LINK)`

2. sudo vim downloads/nginx-1.20.0/src/event/modules/ngx_epoll_module.c
    * add `batch_start();` at line: 835
    * add `batch_flush();` at line: 934

After above modification, compile nginx:
```
make nginx-build
```
Last, replace `/usr/local/nginx/conf/nginx.conf` with `nginx.conf` (make sure root path in line: 19 of nginx.conf be set properly, we provide several static files under `/web`):
```
sudo cp nginx.conf /usr/local/nginx/conf/nginx.conf
```

## Testing

### Lauch Nginx
Choose either
```
make nginx-launch # origin nginx
```
or
```
make insert # insert kernel module
make nginx-esca-launch # nginx-esca
```

### Benchmarking
```
./downloads/wrk-master/wrk -c 50 -d 5s -t 4 http://localhost:8081/a20.html
```
### Demo
![image](https://github.com/eecheng87/dBatch/blob/ngx-demo/demo.gif)

Nginx-ESCA shows 11% improvement compared with Nginx
