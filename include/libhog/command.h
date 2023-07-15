#ifndef COMMAND_H_
#define COMMAND_H_

#include <stddef.h>

enum hog_commands { HOG_COMMAND_SKIP_BYTES };

struct hog_command {
  enum hog_commands type;

  // data the command may require
  union {
    size_t skip_bytes;
  };
};

#endif
