#ifndef COMMAND_H_
#define COMMAND_H_

#include "libhog/types.h"
#include <stddef.h>

enum hog_commands { HOG_COMMAND_SKIP_BYTES };

struct hog_command {
  enum hog_commands type;

  // data the command may require
  union {
    size_t skip_bytes;
    struct {
      const char *fmt_str;
    };
  };
};

// this will get a default formatter for each data type
const char *hog_types_default_fmt(enum hog_types type);

#endif
