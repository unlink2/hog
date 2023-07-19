#ifndef CONFIG_H_
#define CONFIG_H_

#include "libhog/vec.h"

struct hog_config {
  int arch_size;

  // holds all literal type definitions
  struct hog_vec types;
  // holds struct type map
  struct hog_vec types_map;

  // list of all commands
  struct hog_vec cmds;
  // list of key/value map for commands
  struct hog_vec cmds_map;
};

struct hog_config hog_config_init(void);

void hog_config_free(struct hog_config *self);

#endif
