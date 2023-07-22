#include "libhog/command.h"
#include "libhog/vec.h"
#include <stdlib.h>
#include <string.h>

struct hog_cmd hog_cmd_init(enum hog_cmds type, size_t next) {
  struct hog_cmd self;
  memset(&self, 0, sizeof(self));
  self.type = type;
  self.next = next;

  return self;
}

struct hog_cmd hog_cmd_move_init(int move_bytes, size_t next) {
  struct hog_cmd self = hog_cmd_init(HOG_CMD_MOVE_BYTES, next);
  self.move_bytes = move_bytes;

  return self;
}

struct hog_cmd hog_cmd_literal_init(const char *literal, size_t next) {
  struct hog_cmd self = hog_cmd_init(HOG_CMD_FMT_LITERAL, next);
  self.literal = strdup(literal);
  return self;
}

struct hog_cmd hog_cmd_type_init(const char *type_name, size_t next) {
  struct hog_cmd self = hog_cmd_init(HOG_CMD_FMT_TYPE, next);
  self.type_name = strdup(type_name);

  return self;
}

void hog_cmd_free(struct hog_cmd *self) {
  // TODO: free correctly
  switch (self->type) {
  case HOG_CMD_FMT_LITERAL:
    free((void *)self->literal);
    break;
  case HOG_CMD_FMT_TYPE:
    free((void *)self->type_name);
    break;
  default:
    break;
  }
}

void hog_cmd_vec_free(struct hog_vec *self) {
  for (size_t i = 0; i < self->len; i++) {
    hog_cmd_free(hog_vec_get(self, i));
  }
  hog_vec_free(self);
}

struct hog_cmd_map hog_cmd_map_init(const char *name,
                                    const struct hog_cmd *cmd) {
  struct hog_cmd_map self;
  memset(&self, 0, sizeof(self));

  self.name = strdup(name);
  self.cmd = cmd;

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
  if (self->next == HOG_CMD_END_OF_LIST) {
    return NULL;
  }

  return hog_vec_get(list, self->next);
}

void hog_cmd_map_free(struct hog_cmd_map *self) { free((void *)self->name); }

void hog_cmd_map_vec_free(struct hog_vec *self) {
  for (size_t i = 0; i < self->len; i++) {
    hog_cmd_map_free(hog_vec_get(self, i));
  }
  hog_vec_free(self);
}
