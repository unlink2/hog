#ifndef COMMAND_H_
#define COMMAND_H_

#include "libhog/config.h"
#include "libhog/types.h"
#include <stddef.h>

#define HOG_CMD_END_OF_LIST -1

enum hog_cmds {
  HOG_CMD_INVAL = 0,

  // begin a new command list
  HOG_CMD_BEGIN,

  // end the current command
  HOG_CMD_END,

  HOG_CMD_BEGIN_SCOPE,
  HOG_CMD_END_SCOPE,

  // formats a type based on its primive
  // and if applicable its current output format setting
  // in hog_rc (e.g. dec, hex, bin or char or integers)
  HOG_CMD_FMT_TYPE,
  HOG_CMD_FMT_LITERAL,
  // same as literal, but it won't be freed
  HOG_CMD_FMT_STATIC_LITERAL,
  // reads the current name from hog_rc and outputs it
  HOG_CMD_FMT_NAME,

  HOG_CMD_MOVE_BYTES,

  HOG_CMD_SET_INT_FMT,
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
    enum hog_int_fmt int_fmt;
  };
  // next command offset into the initial vec
  // this may be followed safely until the comand type is HOG_CMD_END
  size_t next;
};

struct hog_cmd hog_cmd_init(enum hog_cmds type, size_t next);

struct hog_cmd hog_cmd_move_init(int move_bytes, size_t next);
struct hog_cmd hog_cmd_literal_init(const char *literal, size_t next);
struct hog_cmd hog_cmd_static_literal_init(const char *literal, size_t next);
struct hog_cmd hog_cmd_type_init(const char *type_name, size_t next);

void hog_cmd_free(struct hog_cmd *self);

void hog_cmd_vec_free(struct hog_vec *self);

// key/value map for commands
struct hog_cmd_map {
  const char *name;
  size_t cmd;
};

struct hog_cmd_map hog_cmd_map_init(const char *name, const size_t cmd);

// returns NULL when type is HOG_CMD_END
// otherwise will return the apropriate pointer
const struct hog_cmd *hog_cmd_next(const struct hog_cmd *self,
                                   const struct hog_vec *list);

void hog_cmd_map_free(struct hog_cmd_map *self);
void hog_cmd_map_vec_free(struct hog_vec *self);

#endif
