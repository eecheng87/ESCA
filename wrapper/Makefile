CFLAGS := -Wall -O2
CFLAGS += -shared -fPIC
LDFLAGS = -ldl -lpthread

DLL = libshim.so wrapper.so
all: $(DLL)

libshim.so: shim.c
	$(CC) $(CFLAGS) -o $@ $<

wrapper.so: wrapper.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

.PHONY: clean
clean:
	$(RM) *.o $(DLL)
