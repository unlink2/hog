#ifndef APPLY_H_
#define APPLY_H_

#include "libhog/command.h"
#include "libhog/config.h"
#include "libhog/vec.h"
#include "libhog/buffer.h"
#include <stdint.h>

struct hog_rc {
  const struct hog_config *cfg;
  enum hog_int_fmt int_fmt;
  enum hog_endianess endianess;
  int scope_level;
};

struct hog_rc hog_rc_init(const struct hog_config *cfg);

// read data from the input
int64_t hog_apply_read(struct hog_rc *rc, const uint8_t *input,
                       size_t input_len, size_t offset, size_t read_len);

/**
 * Apply a command list to an input buffer and write the result
 * to buf. offset is the "starting" byte of the operation
 * returns the new offset
 */
size_t hog_apply(struct hog_rc *rc, struct hog_buffer *buf,
                 const uint8_t *input, size_t len, const struct hog_cmd *cmd,
                 size_t offset);

// performs a lookup based on cmd_name and then calls hog_apply
size_t hog_apply_lookup(struct hog_rc *rc, struct hog_buffer *buf,
                        const uint8_t *input, size_t len, const char *cmd_name,
                        size_t offset);

#endif
