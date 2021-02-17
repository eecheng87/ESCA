#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char **argv) {
  char buff[8];
  int ptr = open("test.txt", O_CREAT | O_WRONLY, S_IWUSR | S_IRUSR | S_IRGRP | S_IWGRP | S_IROTH);
  close(ptr);
  ptr = open("test.txt", O_CREAT | O_WRONLY, S_IWUSR | S_IRUSR | S_IRGRP | S_IWGRP | S_IROTH);
  close(ptr);
  read(1, buff, 4);
  write(1, "hihi", 4);
  return 0;
}