all: so

so:
	gcc -shared -o libshim.so -fPIC shim.c
	gcc -shared -fPIC wrapper.c -o wrapper.so -ldl -lpthread


.PHONY: clean wrapper.so
clean:
	rm -f *.o  wrapper.so libshim.so preload.c
