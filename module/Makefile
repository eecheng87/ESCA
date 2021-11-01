obj-m := batch.o
EXTRA_CFLAGS := -I$M/../include

all: scTab.h batch.o
batch.o : scTab.h
	make -C /lib/modules/`uname -r`/build M=$(pwd) modules

scTab.h:
	(grep -w sys_call_table /boot/System.map-`uname -r` | \
	  sed 's/\([^ ]*\) .*/#define smSCTab ((char *)0x\1UL)/'; \
	  sudo grep -w _vdso_data /boot/System.map-`uname -r` | \
      sed 's/\([^ ]*\) .*/#define vdsoDataAddr ((char *)0x\1UL)/'; \
	  sudo grep -w init_mm      /boot/System.map-`uname -r` | \
          sed 's/\([^ ]*\) .*/#define sysMM ((char *)0x\1UL)/'; \
	 grep -w system_wq      /boot/System.map-`uname -r` | \
	  sed 's/\([^ ]*\) .*/#define smSysWQ ((char *)0x\1UL)/') > scTab.h

clean:
	rm -f scTab.h
	make -C /lib/modules/`uname -r`/build M=$(pwd) clean

.PHONY: scTab.h clean
