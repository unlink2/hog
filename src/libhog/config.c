#include "libhog/config.h"
#include "libhog/command.h"
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

struct hog_type_map *hog_config_type_add(struct hog_config *self,
                                         const char *name,
                                         struct hog_type type) {
  struct hog_type *t = hog_vec_push(&self->types, &type);
  struct hog_type_map map = hog_type_map_init(t, name, 0);

  return hog_vec_push(&self->types_map, &map);
}

struct hog_type_map *hog_config_type_lookup(struct hog_config *self,
                                            const char *name) {
  // keep this dumb until it actually causes bad performance
  struct hog_vec *map = &self->types_map;
  size_t name_len = strlen(name);
  for (size_t i = 0; i < map->len; i++) {
    struct hog_type_map *tm = hog_vec_get(map, i);
    if (strncmp(name, tm->name, name_len) == 0) {
      return tm;
    }
  }

  return NULL;
}

struct hog_type_map *hog_config_type_add_alias(struct hog_config *self,
                                               const char *alias_name,
                                               const char *type_name,
                                               int ptr_level) {
  struct hog_type_map *tm = hog_config_type_lookup(self, type_name);
  if (!tm) {
    return NULL;
  }

  // ptr levels are additive to mirror pointers to pointers
  ptr_level += tm->ptr_level;

  struct hog_type_map type_map =
      hog_type_map_init(tm->type, type_name, ptr_level);

  return hog_vec_push(&self->types_map, &type_map);
}

void hog_config_free(struct hog_config *self) {
  hog_type_vec_free(&self->types);
  hog_type_map_vec_free(&self->types_map);

  hog_cmd_map_vec_free(&self->cmds);
  hog_cmd_map_vec_free(&self->cmds_map);
}
