#ifndef CONFIG_H_
#define CONFIG_H_

#include "libhog/vec.h"
#include "libhog/types.h"
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
  int arch_size;

  // holds all literal type definitions
  struct hog_vec types;
  // holds struct type map
  struct hog_vec types_map;

  // list of all commands
  struct hog_vec cmds;
  // list of key/value map for commands
  struct hog_vec cmds_map;

  int16_t indent_cnt;
  char indent_char;
  char scope_open;
  char scope_close;
  char array_open;
  char array_close;
  char array_sep;
  char new_line;
};

struct hog_config hog_config_init(void);

// create built in types and type names
struct hog_config hog_config_init_builtins(void);

// FIXME: raise errors when add or add_alias attempts to add an existing name

void hog_config_def_builtin_type(struct hog_config *self, const char *name,
                                 struct hog_type type);

// adds a new type and an initial type alias
// returns the initial alias
struct hog_type_map *hog_config_type_add(struct hog_config *self,
                                         const char *name,
                                         struct hog_type type);

// creates a new type alias for an existing type name
struct hog_type_map *hog_config_type_add_alias(struct hog_config *self,
                                               const char *alias_name,
                                               const char *type_name);

// look up a type. Returns NULL if no corresponding type was found
const struct hog_type *hog_config_type_lookup(const struct hog_config *self,
                                              const char *name, size_t *index);

// creates a new command
// writes the index of the newly added item into *index unless index is NULL
struct hog_cmd *hog_config_cmd_add(struct hog_config *self, struct hog_cmd cmd,
                                   size_t *index);
// adds a name to an existing command
struct hog_cmd_map *hog_config_cmd_add_alias(struct hog_config *self,
                                             const char *name, size_t cmd);

// look up a command. Returns NULL if no corresponding type was found
const struct hog_cmd *hog_conifg_cmd_lookup(const struct hog_config *self,
                                            const char *name);

void hog_config_free(struct hog_config *self);

#endif
