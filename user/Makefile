all: so test

test:
	gcc -I../include -pthread test.c -o test
so:
	gcc -I../include -shared -fPIC preload.c -o preload.so -ldl


.PHONY: clean test preload.so
clean:
	rm -f *.o test preload.so