WRK_SOURCE := https://github.com/wg/wrk/archive/refs/heads/master.zip
WRK_NAME := wrk-master
WRK_PATH := downloads/wrk-master
WRK := wrk

NGX_SOURCE := http://nginx.org/download/nginx-1.20.0.tar.gz
NGX_NAME := nginx-1.20.0
NGX_PATH := downloads/$(NGX_NAME)
NGX := nginx
OUT := downloads

all: $(WRK)

.PHONY: $(WRK) $(NGX) clean

$(WRK):
	@echo "download wrk..."
	wget $(WRK_SOURCE)
	unzip -d $(OUT) master.zip
	rm master.zip
	sudo $(MAKE) -j4 -C $(OUT)/$(WRK_NAME) all

$(NGX):
	@echo "download nginx..."
	wget $(NGX_SOURCE)
	mkdir $(NGX_PATH)
	tar -zxvf $(NGX_NAME).tar.gz -C $(OUT)
	rm $(NGX_NAME).tar.gz
	cd $(NGX_PATH) && sudo ./configure --prefix=/var/www/html --sbin-path=/usr/sbin/nginx --conf-path=/home/eecheng/Desktop/esca-test/conf/nginx.conf --http-log-path=/var/log/nginx/access.log --error-log-path=/var/log/nginx/error.log --with-pcre  --lock-path=/var/lock/nginx.lock --pid-path=/var/run/nginx.pid --with-http_ssl_module --with-http_image_filter_module=dynamic --modules-path=/etc/nginx/modules --with-http_v2_module --with-stream=dynamic --with-http_addition_module && \
	sudo make && \
	sudo make install


clean:
	rm -rf $(WRK_PATH)
	rm -rf $(NGX_PATH)

