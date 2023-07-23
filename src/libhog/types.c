#include "libhog/types.h"
#include "libhog/log.h"
#include "libhog/vec.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct hog_type hog_type_init(enum hog_types type, const char *name,
                              size_t ptr_to) {
  struct hog_type self;
  memset(&self, 0, sizeof(self));

  self.name = strdup(name);
  self.type = type;
  self.ptr_to_idx = ptr_to;

  return self;
}

int hog_type_ptr_depth(const struct hog_type *self, struct hog_vec *types) {
  const struct hog_type *t = self;
  int i = 0;
  while (t->ptr_to_idx != HOG_NULL_IDX) {
    t = hog_vec_get(types, i++);
  }

  return i;
}

size_t hog_type_sizeof(const struct hog_type *self, size_t arch_len) {
  // pointers always have the arch's length
  if (self->ptr_to_idx != HOG_NULL_IDX) {
    return arch_len;
  }

  switch (self->type) {
  case HOG_TYPE_USIZE:
  case HOG_TYPE_ISIZE:
    return arch_len;
  case HOG_TYPE_U8:
  case HOG_TYPE_I8:
    return 1;
  case HOG_TYPE_U16:
  case HOG_TYPE_I16:
    return 2;
  case HOG_TYPE_U32:
  case HOG_TYPE_I32:
  case HOG_TYPE_F32:
    return 4;
  case HOG_TYPE_U64:
  case HOG_TYPE_I64:
  case HOG_TYPE_F64:
    return 8;
  case HOG_TYPE_ARRAY:
    // arrays do not have a size on their own, but their content does
    return 0;
  default:
    hog_warn("sizeof is not implemented for type: %d\n", self->type);
  }

  return 0;
}

void hog_type_free(struct hog_type *self) { free((void *)self->name); }

void hog_type_vec_free(struct hog_vec *self) {
  for (size_t i = 0; i < self->len; i++) {
    hog_type_free(hog_vec_get(self, i));
  }
  hog_vec_free(self);
}

struct hog_type_map hog_type_map_init(const size_t type, const char *name) {
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
