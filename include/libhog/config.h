#ifndef CONFIG_H_
#define CONFIG_H_

#include <stddef.h>
#include <stdint.h>
#include "libhog/io.h"

struct hog_config {
  size_t mem_size;

  const char *fin_path;

  struct hog_vm *vm;

  int log_level;
};

struct hog_config hog_config_init(void);

void hog_config_build(struct hog_config *self);

#endif
