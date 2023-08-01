#ifndef CONFIG_H_
#define CONFIG_H_

#include <stddef.h>
#include <stdint.h>

enum hog_int_fmt {
  HOG_FMT_INT_DEC = 0,
  HOG_FMT_INT_HEX,
  HOG_FMT_INT_BIN,
  HOG_FMT_INT_CHAR,
};

enum hog_endianess {
  HOG_ENDIAN_LITTLE,
  HOG_ENDIAN_BIG,
};

struct hog_config {
  size_t mem_size;
};

struct hog_config hog_config_init(void);

void hog_config_free(struct hog_config *self);

#endif
