#include "libhog/command.h"
#include "libhog/vec.h"
#include <stdio.h>
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

struct hog_cmd hog_cmd_static_literal_init(const char *literal, size_t next) {
  struct hog_cmd self = hog_cmd_init(HOG_CMD_FMT_STATIC_LITERAL, next);
  self.literal = literal;
  return self;
}

struct hog_cmd hog_cmd_type_init(const size_t type_idx, size_t next) {
  struct hog_cmd self = hog_cmd_init(HOG_CMD_FMT_TYPE, next);
  self.type_idx = type_idx;

  return self;
}

void hog_cmd_free(struct hog_cmd *self) {
  // TODO: free correctly
  switch (self->type) {
  case HOG_CMD_FMT_LITERAL:
    free((void *)self->literal);
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

struct hog_cmd_map hog_cmd_map_init(const char *name, const size_t cmd) {
  struct hog_cmd_map self;
  memset(&self, 0, sizeof(self));

  self.name = strdup(name);
  self.cmd = cmd;

  return self;
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
