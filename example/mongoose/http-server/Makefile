PROG ?= example
ROOT ?= $(realpath $(CURDIR)/../..)
DEFS ?= -DMG_ENABLE_LINES=1 -DMG_ENABLE_DIRECTORY_LISTING=1 -DMG_ENABLE_SSI=1
CFLAGS ?= -I../.. -W -Wall -DMG_ENABLE_IPV6=1 $(DEFS) $(EXTRA)
VCFLAGS = /nologo /W3 /O2 /I../.. $(DEFS) $(EXTRA) /link /incremental:no /machine:IX86
VC98 = docker run --rm -e Tmp=. -v $(ROOT):$(ROOT) -w $(CURDIR) docker.io/mdashnet/vc98

all: $(PROG)
	LD_PRELOAD=$(PWD)/../../../user/preload.so ./example

$(PROG): main.c Makefile
	gcc ../mongoose.c -I../ -L$(PWD)/../../../user -Wl,-rpath=$(PWD)/../../../user -o example main.c -ldummy -lpthread

clean:
	rm -rf $(PROG) *.o *.dSYM *.gcov *.gcno *.gcda *.obj *.exe *.ilk *.pdb mongoose mongoose_* mongoose.*
