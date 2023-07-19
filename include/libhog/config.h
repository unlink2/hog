#ifndef CONFIG_H_
#define CONFIG_H_

#include "libhog/vec.h"
#include "libhog/types.h"

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

// adds a new type and an initial type alias
// returns the initial alias
struct hog_type_map *hog_config_type_add(struct hog_config *self,
                                         const char *name,
                                         struct hog_type type);

// creates a new type alias for an existing type name
struct hog_type_map *hog_config_type_add_alias(struct hog_config *self,
                                               const char *alias_name,
                                               const char *type_name,
                                               int ptr_level);

struct hog_type_map *hog_config_type_lookup(struct hog_config *self,
                                            const char *name);

// creates a new command
struct hog_cmd *hog_config_cmd_add(struct hog_config *self, struct hog_cmd cmd,
                                   size_t next);
// adds a name to an existing command
struct hog_cmd_map *hog_config_cmd_add_alias(struct hog_config *self,
                                             const char *name,
                                             struct hog_cmd *cmd);

void hog_config_free(struct hog_config *self);

#endif
