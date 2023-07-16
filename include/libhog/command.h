#ifndef COMMAND_H_
#define COMMAND_H_

#include "libhog/types.h"
#include <stddef.h>

enum hog_cmds {
  HOG_CMD_INVAL = 0,
  HOG_CMD_BEGIN,
  HOG_CMD_END,

  HOG_CMD_BEGIN_SCOPE,
  HOG_CMD_END_SCOPE,

  HOG_CMD_FMT_PRIMITIVE,

  HOG_CMD_MOVE_BYTES,

};

struct hog_cmd {
  enum hog_cmds type;

  // data the command may require
  union {
    int move_bytes;
  };
};

struct hog_cmd hog_cmd_init(void);

struct hog_cmd hog_cmd_move_init(int move_bytes);

#endif
