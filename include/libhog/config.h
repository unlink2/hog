#ifndef CONFIG_H_
#define CONFIG_H_

#include "libhog/vec.h"

struct hog_config {
  int arch_size;

  // holds all literal type definitions
  struct hog_vec types;

  // holds struct type map
  struct hog_vec types_map;
};

struct hog_config hog_config_init(void);

#endif
