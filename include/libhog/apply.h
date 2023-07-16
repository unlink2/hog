#ifndef APPLY_H_
#define APPLY_H_

#include "libhog/command.h"
#include "libhog/vec.h"
#include "libhog/buffer.h"

/**
 * Apply a command to an input buffer and write the result
 * to buf.
 * returns the amount of bytes read from input
 */
size_t hog_apply(struct hog_buffer *buf, const uint8_t *input, size_t len,
                 struct hog_command cmd, size_t offset);

#endif
