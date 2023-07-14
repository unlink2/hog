#ifndef APPLY_H_
#define APPLY_H_

#include "libhog/vec.h"
#include "libhog/buffer.h"

/**
 * Apply a data type to an input and write the result
 * to buf
 */
size_t hog_apply_type(struct hog_buffer *buf, const uint8_t *input);

#endif
