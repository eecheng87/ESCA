ALL :=  write dwrite

all: $(ALL)

CC := gcc
C := $(CC) -g -I../include

% : %.c
	$C $< -o $@ -lpthread

clean:
	$(RM) $(ALL)

PHONY: all clean