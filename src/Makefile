all: so test

test:
	gcc test.c -o test
so:
	gcc -shared -fPIC preload.c -o preload.so -ldl


.PHONY: clean test preload.so
clean:
	rm -f *.o test preload.so