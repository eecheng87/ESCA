all: so

so:
	gcc -shared -o libdummy.so -fPIC dummy.c
	gcc -shared -fPIC preload.c -o preload.so -ldl -lpthread


.PHONY: clean preload.so
clean:
	rm -f *.o  preload.so libdummy.so
