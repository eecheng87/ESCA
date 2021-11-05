all: so

so:
	gcc -shared -o libshim.so -fPIC shim.c
	gcc -shared -fPIC preload.c -o preload.so -ldl -lpthread


.PHONY: clean preload.so
clean:
	rm -f *.o  preload.so libshim.so
