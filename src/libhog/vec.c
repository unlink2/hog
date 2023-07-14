#include "libhog/vec.h"
#include "libhog/error.h"
#include "libhog/macros.h"
#include "libhog/log.h"
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

struct hog_vec hog_vec_init(size_t stride) {
  struct hog_vec self;
  memset(&self, 0, sizeof(self));

  self.stride = stride;
  self.max_len = hog_VEC_DEFAULT_MAX_LEN;
  self.data = malloc(self.stride * self.max_len);
  memset(self.data, 0, self.stride * self.max_len);

  return self;
}

#define hog_vec_offset(self, index)                                            \
  ((uint8_t *)(self)->data + (self)->stride * (index))

void hog_vec_resize(struct hog_vec *self) {
  self->max_len = self->max_len * 2;
  void *new_data = realloc(self->data, self->stride * self->max_len);

  if (new_data) {
    self->data = new_data;
  } else {
    hog_errno();
  }
}

void hog_vec_push(struct hog_vec *self, void *item) {
  if (self->len >= self->max_len) {
    hog_vec_resize(self);
    if (hog_err()) {
      hog_error("Unable to resize vec %p!\n", (void *)self);
      return;
    }
  }

  memcpy(hog_vec_offset(self, self->len), item, self->stride);
  self->len++;
}

void *hog_vec_pop(struct hog_vec *self) {
  void *d = hog_vec_get(self, self->len - 1);
  if (d) {
    self->len--;
  }
  return d;
}

void *hog_vec_get(struct hog_vec *self, size_t index) {
  if (self->len <= index) {
    return NULL;
  }
  return hog_vec_offset(self, index);
}

void hog_vec_clear(struct hog_vec *self) { self->len = 0; }

void hog_vec_free(struct hog_vec *self) { free(self->data); }
