all: so

so:
	gcc -shared -o libshim.so -fPIC shim.c
	gcc -shared -fPIC wrapper.c -o wrapper.so -ldl -lpthread


.PHONY: clean wrapper.so
clean:
	$(RM) *.o  wrapper.so libshim.so
