#include "libhog/buffer.h"
#include "libhog/error.h"
#include "libhog/log.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct hog_buffer hog_buffer_init(void) {
  struct hog_buffer self;
  memset(&self, 0, sizeof(self));

  self.data = malloc(HOG_BUFFER_INITIAL_LEN);
  memset(self.data, 0, HOG_BUFFER_INITIAL_LEN);
  self.max_len = HOG_BUFFER_INITIAL_LEN;

  return self;
}

void hog_buffer_resize(struct hog_buffer *self, size_t by) {
  size_t new_max_len = self->max_len + by;
  uint8_t *new_data = realloc(self->data, new_max_len);

  if (new_data) {
    hog_dbg("Resized buffer %p:%ld by %ld bytes (to %ld). Old address: %p. New "
            "address: %p\n",
            (void *)self, self->max_len, by, new_max_len, (void *)self->data,
            (void *)new_data);
    self->max_len = new_max_len;
    self->data = new_data;
  } else {
    hog_errno();
  }
}

uint8_t *hog_buffer_next(struct hog_buffer *self, size_t len) {
  if (self->index + len >= self->max_len) {
    hog_buffer_resize(self, len * 2);
    if (hog_err()) {
      return NULL;
    }
  }

  return self->data + self->index;
}

void hog_buffer_adv(struct hog_buffer *self, size_t n) { self->index += n; }

void hog_buffer_clear(struct hog_buffer *self) { self->index = 0; }

const uint8_t *hog_buffer_start(struct hog_buffer *self) {
  hog_dbg_assert(self->data);
  return self->data;
}

size_t hog_buffer_len(struct hog_buffer *self) { return self->index; }

void hog_buffer_null_term(struct hog_buffer *self) {
  uint8_t *next = hog_buffer_next(self, 1);
  *next = '\0';
}

uint8_t *hog_buffer_move(struct hog_buffer *self) { return self->data; }

uint8_t *hog_buffer_cat(char separator, const char **strings, size_t len) {
  struct hog_buffer self = hog_buffer_init();
  for (size_t i = 0; i < len; i++) {
    size_t len = strlen(strings[i]);
    uint8_t *next = hog_buffer_next(&self, len);
    memcpy(next, strings[i], len);
    hog_buffer_adv(&self, len);

    // do not append \0 and also do not append
    // when the next element is the last
    if (separator) {
      // space between all the commands
      *hog_buffer_next(&self, 1) = separator;
      hog_buffer_adv(&self, 1);
    }
  }
  hog_buffer_null_term(&self);

  return hog_buffer_move(&self);
}

void hog_buffer_free(struct hog_buffer *self) { free(self->data); }
