#include <unistd.h>
#include "libhog/io.h"

size_t hog_std_read(int fd, void *d, size_t n) { return read(fd, d, n); }

size_t hog_std_write(int fd, const void *d, size_t n) {
  return write(fd, d, n);
}
