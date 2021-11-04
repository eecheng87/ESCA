WRK_SOURCE := https://github.com/wg/wrk/archive/refs/heads/master.zip
WRK_NAME := wrk-master
WRK_PATH := downloads/$(WRK_NAME)
WRK := wrk

NGX_SOURCE := http://nginx.org/download/nginx-1.20.0.tar.gz
NGX_NAME := nginx-1.20.0
NGX_PATH := downloads/$(NGX_NAME)
NGX := nginx

LIGHTY_SOURCE := https://github.com/lighttpd/lighttpd1.4/archive/refs/tags/lighttpd-1.4.58.tar.gz
LIGHTY_ZIP_NAME := lighttpd-1.4.58
LIGHTY_NAME := lighttpd1.4-lighttpd-1.4.58
LIGHTY_PATH := downloads/$(LIGHTY_NAME)
LIGHTY := lighttpd

LIBDUMMY_PATH := $(shell find $(shell pwd) -type f -name "libdummy.so") | sed 's_/_\\/_g'

OUT := downloads

all: module wrapper

.PHONY: $(WRK) $(NGX) $(LIGHTY) config module wrapper clean

ifeq ($(strip $(TARGET)),nginx)
TARGET = nginx
else
TARGET = lighttpd
endif

config:
	ln -s $(shell pwd)/wrapper/$(TARGET)-preload.c wrapper/preload.c
	ln -s $(shell pwd)/module/$(TARGET)-batch.c module/batch.c

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
	sh ngx.sh $(NGX_PATH)
	cd $(OUT) && patch -p1 < ../ngx.patch
	cd $(NGX_PATH) && make && \
	sudo make install
	sudo cp nginx.conf /usr/local/nginx/conf/nginx.conf

$(LIGHTY):
	@echo "download lighttpd..."
	wget $(LIGHTY_SOURCE)
	tar -zxvf $(LIGHTY_ZIP_NAME).tar.gz -C $(OUT)
	rm $(LIGHTY_ZIP_NAME).tar.gz
	cd $(LIGHTY_PATH) && ./autogen.sh && ./configure
	sh lighttpd.sh $(LIGHTY_PATH)
	cd $(OUT) && patch -p1 < ../lighttpd.patch
	cd $(LIGHTY_PATH) && sudo make install
	cp lighttpd.conf $(LIGHTY_PATH)/src/lighttpd.conf

nginx-launch:
	sudo ./downloads/nginx-1.20.0/objs/nginx

nginx-esca-launch:
	sudo LD_PRELOAD=wrapper/preload.so ./downloads/nginx-1.20.0/objs/nginx

lighttpd-launch:
	./$(LIGHTY_PATH)/src/lighttpd -D -f $(LIGHTY_PATH)/src/lighttpd.conf

lighttpd-esca-launch:
	LD_PRELOAD=wrapper/preload.so ./$(LIGHTY_PATH)/src/lighttpd -D -f $(LIGHTY_PATH)/src/lighttpd.conf

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
	rm -rf $(LIGHTY_PATH)
	rm wrapper/preload.c
	rm module/batch.c
	$(MAKE) -C module $(MAKECMDGOALS)
	$(MAKE) -C wrapper $(MAKECMDGOALS)