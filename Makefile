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
PWD := $(shell pwd)

OUT := downloads

all: lkm wrapper

.PHONY: $(WRK) $(NGX) $(LIGHTY) config lkm wrapper clean

ifeq ($(strip $(TARGET)),nginx)
TARGET = nginx
else
TARGET = lighttpd
endif

config:
	-$(RM) wrapper/preload.c
	(cd wrapper ; ln -s $(TARGET)-preload.c preload.c)
	touch $@

$(WRK):
	@echo "download wrk..."
	wget $(WRK_SOURCE)
	unzip -d $(OUT) master.zip
	$(RM) master.zip
	sudo $(MAKE) -j4 -C $(OUT)/$(WRK_NAME) all

$(NGX):
	@echo "download nginx..."
	wget $(NGX_SOURCE)
	mkdir $(NGX_PATH)
	tar -zxvf $(NGX_NAME).tar.gz -C $(OUT)
	$(RM) $(NGX_NAME).tar.gz
	mkdir local
	cd $(NGX_PATH) && ./configure --prefix=$(PWD)/local
	scripts/ngx.sh $(NGX_PATH)
	cd $(OUT) && patch -p1 < ../patches/ngx_module.patch && patch -p1 < ../patches/ngx_process.patch
	cd $(NGX_PATH) && make && \
	make install
	cp -f configs/nginx.conf local/conf/nginx.conf

$(LIGHTY):
	@echo "download lighttpd..."
	wget $(LIGHTY_SOURCE)
	tar -zxvf $(LIGHTY_ZIP_NAME).tar.gz -C $(OUT)
	$(RM) $(LIGHTY_ZIP_NAME).tar.gz
	cd $(LIGHTY_PATH) && ./autogen.sh && ./configure --without-pcre
	scripts/lighttpd.sh $(LIGHTY_PATH)
	cd $(OUT) && patch -p1 < ../patches/lighttpd.patch
	cd $(LIGHTY_PATH) && sudo make install
	cp -f configs/lighttpd.conf $(LIGHTY_PATH)/src/lighttpd.conf

nginx-launch:
	./downloads/nginx-1.20.0/objs/nginx

nginx-esca-launch:
	LD_PRELOAD=wrapper/wrapper.so ./downloads/nginx-1.20.0/objs/nginx

lighttpd-launch:
	./$(LIGHTY_PATH)/src/lighttpd -D -f $(LIGHTY_PATH)/src/lighttpd.conf

lighttpd-esca-launch:
	LD_PRELOAD=wrapper/wrapper.so ./$(LIGHTY_PATH)/src/lighttpd -D -f $(LIGHTY_PATH)/src/lighttpd.conf

lkm: config
	sudo $(MAKE) -C $@ $(MAKECMDGOALS)

wrapper: config
	$(MAKE) -C $@ $(MAKECMDGOALS)

load-lkm:
	sudo insmod lkm/esca.ko

unload-lkm:
	sudo rmmod esca

clean:
	rm -rf $(WRK_PATH)
	rm -rf $(NGX_PATH)
	rm -rf $(LIGHTY_PATH)
	rm -rf local
	$(MAKE) -C lkm clean
	$(MAKE) -C wrapper clean

distclean: clean
	$(RM) wrapper/preload.c
	-$(RM) config
