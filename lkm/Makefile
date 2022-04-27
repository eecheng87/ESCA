obj-m := esca.o
EXTRA_CFLAGS := -I$M/../include
EXTRA_CFLAGS += -D'DEBUG=0'

all: syscall-table.h esca.o
esca.o : syscall-table.h
	# $(shell pwd) is for zsh, $(PWD) seems good for bash
	make -C /lib/modules/`uname -r`/build M=$(shell pwd) modules

syscall-table.h:
	(grep -w sys_call_table /boot/System.map-`uname -r` | \
	  sed 's/\([^ ]*\) .*/#define smSCTab ((char *)0x\1UL)/'; \
	 grep -w system_wq      /boot/System.map-`uname -r` | \
	  sed 's/\([^ ]*\) .*/#define smSysWQ ((char *)0x\1UL)/') > syscall-table.h

clean:
	rm -f syscall-table.h
	make -C /lib/modules/`uname -r`/build M=$(shell pwd) clean

.PHONY: syscall-table.h clean
