#ifndef APPLY_H_
#define APPLY_H_

#include "libhog/command.h"
#include "libhog/config.h"
#include "libhog/vec.h"
#include "libhog/buffer.h"

struct hog_rc {
  const struct hog_config *cfg;
  int scope_level;
  enum hog_int_fmt int_fmt;
  enum hog_endianess endianess;
};

struct hog_rc hog_rc_init(const struct hog_config *cfg);

/**
 * Apply a command to an input buffer and write the result
 * to buf. offset is the "starting" byte of the operation
 * returns the new offset
 */
size_t hog_apply(struct hog_rc *rc, struct hog_buffer *buf,
                 const uint8_t *input, size_t len, struct hog_cmd *cmd,
                 size_t offset);

#endif
