#include "libhog/types.h"
#include "libhog/vec.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct hog_type hog_type_init(enum hog_types type, const char *name,
                              struct hog_type *ptr_to) {
  struct hog_type self;
  memset(&self, 0, sizeof(self));

  self.name = strdup(name);
  self.type = type;
  self.ptr_to = ptr_to;

  return self;
}

void hog_type_free(struct hog_type *self) { free((void *)self->name); }

void hog_type_vec_free(struct hog_vec *self) {
  for (size_t i = 0; i < self->len; i++) {
    hog_type_free(hog_vec_get(self, i));
  }
  hog_vec_free(self);
}

struct hog_type_map hog_type_map_init(const struct hog_type *type,
                                      const char *name) {
  struct hog_type_map self;
  memset(&self, 0, sizeof(self));

  self.name = strdup(name);
  self.type = type;

  return self;
}

void hog_type_map_free(struct hog_type_map *self) { free((void *)self->name); }

void hog_type_map_vec_free(struct hog_vec *self) {
  for (size_t i = 0; i < self->len; i++) {
    hog_type_map_free(hog_vec_get(self, i));
  }
  hog_vec_free(self);
}
