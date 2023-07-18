#include "libhog/command.h"
#include <string.h>

struct hog_cmd hog_cmd_init(void) {
  struct hog_cmd self;
  memset(&self, 0, sizeof(self));

  return self;
}

struct hog_cmd hog_cmd_move_init(int move_bytes) {
  struct hog_cmd self = hog_cmd_init();
  self.move_bytes = move_bytes;
  self.type = HOG_CMD_MOVE_BYTES;

  return self;
}

const struct hog_cmd *hog_cmd_lookup(const struct hog_vec *self,
                                     const char *name) {
  size_t name_len = strlen(name);
  for (size_t i = 0; i < self->len; i++) {
    struct hog_cmd_map *entry = hog_vec_get(self, i);
    if (strncmp(name, entry->name, name_len) == 0) {
      return entry->cmd;
    }
  }

  return NULL;
}

const struct hog_cmd *hog_cmd_next(const struct hog_cmd *self,
                                   const struct hog_vec *list) {
  if (self->type == HOG_CMD_END) {
    return NULL;
  }

  return hog_vec_get(list, self->next);
}
