# How to deploy ESCA to Nginx and lighttpd
This branch gives code which can simply show how nginx-esca and lighttpd-esca effectively improve throughput (this version only works for x86)

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

## Config demo target
Choose either Nginx:
```
make config TARGET=nginx
```
or lighttpd:
```
make config TARGET=lighttpd
```

## Build ESCA
Compile files under `/module` and `/wrapper` (This step must be taken before next step)
```
sudo make
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
Download & build lighttpd
```
sudo make lighttpd
```

## Testing

### Launch Nginx
Choose either
```
make nginx-launch # origin nginx
```
or
```
make insert # insert kernel module
make nginx-esca-launch # nginx-esca
```
### Launch lighttpd
Choose either
```
make lighttpd-launch # origin lighttpd
```
or
```
make insert # insert kernel module
make lighttpd-esca-launch # lighttpd-esca
```

### Benchmarking
```
# nginx is at port 8081; lighttpd is at port 3000
./downloads/wrk-master/wrk -c 50 -d 5s -t 4 http://localhost:8081/a20.html
```
### Demo
![image](https://github.com/eecheng87/dBatch/blob/ngx-demo/demo.gif)

Nginx-ESCA shows 11% improvement compared with Nginx

![image](https://github.com/eecheng87/dBatch/blob/ngx-demo/light-demo.gif)

lighttpd-ESCA shows 13% improvement compared with lighttpd

