obj-m := batch.o
EXTRA_CFLAGS := -I$M/../include

all: scTab.h batch.o
batch.o : scTab.h
	# $(shell pwd) is for zsh, $(PWD) seems good for bash
	make -C /lib/modules/`uname -r`/build M=$(shell pwd) modules

scTab.h:
	(grep -w sys_call_table /boot/System.map-`uname -r` | \
	  sed 's/\([^ ]*\) .*/#define smSCTab ((char *)0x\1UL)/'; \
	 grep -w system_wq      /boot/System.map-`uname -r` | \
	  sed 's/\([^ ]*\) .*/#define smSysWQ ((char *)0x\1UL)/') > scTab.h

clean:
	rm -f scTab.h
	make -C /lib/modules/`uname -r`/build M=$(shell pwd) clean

.PHONY: scTab.h clean
