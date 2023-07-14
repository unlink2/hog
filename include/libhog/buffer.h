#ifndef BUFFER_H_
#define BUFFER_H_

#include <stddef.h>
#include <stdint.h>

#define HOG_BUFFER_INITIAL_LEN 128

/**
 * Simple expanding string buffer
 */
struct hog_buffer {
  uint8_t *data;
  size_t index;
  size_t max_len;
};

struct hog_buffer hog_buffer_init(void);

// reserve the next 'len' bytes
// for use - if the buffer does not have enough data at this point
// it will resize to fit twice the requested length
// and return the apropriate pointer
uint8_t *hog_buffer_next(struct hog_buffer *self, size_t len);

// advance index by n bytes
void hog_buffer_adv(struct hog_buffer *self, size_t n);

void hog_buffer_clear(struct hog_buffer *self);

// returns the start address of the current buffer ptr
const uint8_t *hog_buffer_start(struct hog_buffer *self);
size_t hog_buffer_len(struct hog_buffer *self);

// this call NULL terminates the buffer by appending \0
// This will *not* advance the buffer. it simply ensures
// termination
void hog_buffer_null_term(struct hog_buffer *self);

// moves the data owned by the buffer out of the buffer
// and guarantees to clean up the remaining data the buffer might use
// using the buffer after this call is ub.
// The caller is responsible for calling free() on the returned
// data
uint8_t *hog_buffer_move(struct hog_buffer *self);

uint8_t *hog_buffer_cat(char separator, const char **strings, size_t len);

void hog_buffer_free(struct hog_buffer *self);

#endif
