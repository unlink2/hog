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

  HOG_CMD_FMT_TYPE,
  HOG_CMD_FMT_LITERAL,

  HOG_CMD_MOVE_BYTES,
};

// a command is a single instruction
// that tells the apply formatter what to output
// into the buffer
// a collection of commands forms a struct
struct hog_cmd {
  enum hog_cmds type;

  // data the command may require
  union {
    int move_bytes;
    const char *literal;

    // names are used for lookup of definitions in config
    const char *type_name;
  };
  // next command offset into the initial vec
  // this may be followed safely until the comand type is HOG_CMD_END
  int next;
};

// key/value map for commands
struct hog_cmd_map {
  const char *name;
  const struct hog_cmd *cmd;
};

struct hog_cmd hog_cmd_init(void);

struct hog_cmd hog_cmd_move_init(int move_bytes);

// looks up a list of commands
// returns the first command from the vec
struct hog_cmd *hog_cmd_lookup(const struct hog_vec *self, const char *name);

// returns NULL when type is HOG_CMD_END
// otherwise will return the apropriate pointer
struct hog_cmd *hog_cmd_next(const struct hog_cmd *self,
                             const struct hog_vec *list);

#endif
