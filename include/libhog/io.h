#ifndef HOG_IO_
#define HOG_IO_

#include <stddef.h>
#include <stdint.h>

typedef size_t (*hog_read)(int fd, void *d, size_t n);
typedef size_t (*hog_write)(int fd, const void *d, size_t n);

size_t hog_std_read(int fd, void *d, size_t n);
size_t hog_std_write(int fd, const void *d, size_t n);

#endif
