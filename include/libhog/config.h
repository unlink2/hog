#ifndef CONFIG_H_
#define CONFIG_H_

#include <stddef.h>
#include <stdint.h>
#include "libhog/io.h"

struct hog_config {
  size_t mem_size;

  int stdin;
  int stdout;
  int fin;
};

struct hog_config hog_config_init(void);

void hog_config_free(struct hog_config *self);

#endif
