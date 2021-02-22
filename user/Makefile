all: so test

test:
	gcc -I../include test.c -o test -lpthread
so:
	gcc -I../include -shared -fPIC preload.c -o preload.so -ldl -lpthread


.PHONY: clean test preload.so
clean:
	rm -f *.o test preload.so