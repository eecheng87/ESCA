WRK_SOURCE := https://github.com/wg/wrk/archive/refs/heads/master.zip
WRK_NAME := wrk-master
WRK_PATH := downloads/$(WRK_NAME)
WRK := wrk

NGX_SOURCE := http://nginx.org/download/nginx-1.20.0.tar.gz
NGX_NAME := nginx-1.20.0
NGX_PATH := downloads/$(NGX_NAME)
NGX := nginx

LIBDUMMY_PATH := $(shell find $(shell pwd) -type f -name "libdummy.so") | sed 's_/_\\/_g'

OUT := downloads

all: module wrapper

.PHONY: $(WRK) $(NGX) module wrapper clean

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
	cd $(NGX_PATH) && ./configure
	sed -i 's/-Werror//' objs/Makefile
	sed -i 's/-Wl,-E/-Wl,-E $(LIBDUMMY_PATH)/' objs/Makefile
	cd $(OUT) && patch -p1 < ../ngx.patch

nginx-build:
	cd $(NGX_PATH) && sudo make && \
	sudo make install

nginx-launch:
	sudo ./downloads/nginx-1.20.0/objs/nginx

nginx-esca-launch:
	sudo LD_PRELOAD=wrapper/preload.so ./downloads/nginx-1.20.0/objs/nginx

module:
	$(MAKE) -C $@ $(MAKECMDGOALS)

wrapper:
	$(MAKE) -C $@ $(MAKECMDGOALS)

insert:
	sudo insmod module/batch.ko

rmmod:
	sudo rmmod batch

clean:
	rm -rf $(WRK_PATH)
	rm -rf $(NGX_PATH)
	$(MAKE) -C module $(MAKECMDGOALS)
	$(MAKE) -C wrapper $(MAKECMDGOALS)