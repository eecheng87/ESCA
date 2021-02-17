#include "preload.h"

int open(const char *pathname, int flags, mode_t mode) {
  fprintf(stderr, "called open()\n");
  if (!real_open)
    real_open = dlsym(RTLD_NEXT, "open");

  return real_open(pathname, flags, mode);
}

int close(int fd) {
  fprintf(stderr, "called close\n");
  if (!real_close)
    real_close = dlsym(RTLD_NEXT, "write");

  return real_close(fd);
}

ssize_t write(int fd, const void *buf, size_t count) {
  fprintf(stderr, "called write()\n");
  if (!real_write)
    real_write = dlsym(RTLD_NEXT, "write");

  return real_write(fd, buf, count);
}

ssize_t read(int fd, void *buf, size_t count) {
  fprintf(stderr, "called read()\n");
  if (!real_read)
    real_read = dlsym(RTLD_NEXT, "read");

  return real_read(fd, buf, count);
}

__attribute__((constructor)) static void setup(void) {
  fprintf(stderr, "called setup()\n");
}