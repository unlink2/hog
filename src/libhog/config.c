#include "libhog/config.h"
#include "libhog/command.h"
#include "libhog/macros.h"
#include "libhog/types.h"
#include "libhog/vec.h"
#include <string.h>

struct hog_config hog_config_init(void) {
  struct hog_config self;
  memset(&self, 0, sizeof(self));

  self.cmds = hog_vec_init(sizeof(struct hog_cmd));
  self.cmds_map = hog_vec_init(sizeof(struct hog_cmd_map));

  self.types = hog_vec_init(sizeof(struct hog_type));
  self.types_map = hog_vec_init(sizeof(struct hog_type_map));

  return self;
}

void hog_config_def_builtin(struct hog_config *self, const char *name,
                            enum hog_types type) {
  hog_config_type_add(self, name, hog_type_init(type, name, HOG_NULL_IDX));

  size_t index = HOG_NULL_IDX;
  struct hog_cmd *cmd = hog_config_cmd_add(
      self, hog_cmd_init(HOG_CMD_FMT_LITERAL, index), &index);
  hog_config_cmd_add(self, hog_cmd_init(HOG_CMD_FMT_TYPE, index), &index);
  hog_config_cmd_add_alias(self, name, cmd);
}

struct hog_config hog_config_init_builtins(void) {
  struct hog_config self = hog_config_init();

  hog_config_def_builtin(&self, "u8", HOG_TYPE_U8);
  hog_config_def_builtin(&self, "u16", HOG_TYPE_U16);
  hog_config_def_builtin(&self, "u32", HOG_TYPE_U32);
  hog_config_def_builtin(&self, "u64", HOG_TYPE_U64);
  hog_config_def_builtin(&self, "usize", HOG_TYPE_USIZE);

  hog_config_def_builtin(&self, "i8", HOG_TYPE_I8);
  hog_config_def_builtin(&self, "i16", HOG_TYPE_I16);
  hog_config_def_builtin(&self, "i32", HOG_TYPE_I32);
  hog_config_def_builtin(&self, "i64", HOG_TYPE_I64);
  hog_config_def_builtin(&self, "isize", HOG_TYPE_ISIZE);

  hog_config_def_builtin(&self, "f32", HOG_TYPE_F32);
  hog_config_def_builtin(&self, "f64", HOG_TYPE_F64);

  hog_config_def_builtin(&self, "void", HOG_TYPE_VOID);

  return self;
}

struct hog_type_map *hog_config_type_add(struct hog_config *self,
                                         const char *name,
                                         struct hog_type type) {
  struct hog_type *t = hog_vec_push(&self->types, &type);
  struct hog_type_map map = hog_type_map_init(t, name);

  return hog_vec_push(&self->types_map, &map);
}

const struct hog_type *hog_config_type_lookup(struct hog_config *self,
                                              const char *name) {
  // keep this dumb until it actually causes bad performance
  struct hog_vec *map = &self->types_map;
  size_t name_len = strlen(name);
  for (size_t i = 0; i < map->len; i++) {
    struct hog_type_map *tm = hog_vec_get(map, i);
    if (strncmp(name, tm->name, name_len) == 0) {
      return tm->type;
    }
  }

  return NULL;
}

struct hog_type_map *hog_config_type_add_alias(struct hog_config *self,
                                               const char *alias_name,
                                               const char *type_name) {
  const struct hog_type *tm = hog_config_type_lookup(self, type_name);
  if (!tm) {
    return NULL;
  }

  struct hog_type_map type_map = hog_type_map_init(tm, type_name);

  return hog_vec_push(&self->types_map, &type_map);
}

struct hog_cmd *hog_config_cmd_add(struct hog_config *self, struct hog_cmd cmd,
                                   size_t *index) {
  struct hog_cmd *c = hog_vec_push(&self->cmds_map, &cmd);

  // write the latest index to index return
  if (index) {
    *index = self->cmds_map.len - 1;
  }

  return c;
}

struct hog_cmd_map *hog_config_cmd_add_alias(struct hog_config *self,
                                             const char *name,
                                             struct hog_cmd *cmd) {
  struct hog_cmd_map m = hog_cmd_map_init(name, cmd);
  return hog_vec_push(&self->cmds_map, &m);
}

const struct hog_cmd *hog_confg_cmd_lookup(struct hog_config *self,
                                           const char *name) {
  struct hog_vec *map = &self->cmds_map;
  size_t name_len = strlen(name);
  for (size_t i = 0; i < map->len; i++) {
    struct hog_cmd_map *cm = hog_vec_get(map, i);
    if (strncmp(name, cm->name, name_len) == 0) {
      return cm->cmd;
    }
  }

  return NULL;
}

void hog_config_free(struct hog_config *self) {
  hog_type_vec_free(&self->types);
  hog_type_map_vec_free(&self->types_map);

  hog_cmd_map_vec_free(&self->cmds);
  hog_cmd_map_vec_free(&self->cmds_map);
}
