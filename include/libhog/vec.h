#ifndef VEC_H_
#define VEC_H_

#include <stddef.h>

#define hog_VEC_DEFAULT_MAX_LEN 16

struct hog_vec {
  void *data;
  size_t stride;
  size_t max_len;
  size_t len;
};

struct hog_vec hog_vec_init(size_t stride);

void *hog_vec_push(struct hog_vec *self, void *item);
void *hog_vec_pop(struct hog_vec *self);

void *hog_vec_get(const struct hog_vec *self, size_t index);

void hog_vec_clear(struct hog_vec *self);

void hog_vec_free(struct hog_vec *self);
void hog_buf_vec_free(struct hog_vec *self);
void hog_words_vec_free(struct hog_vec *self);

#endif
