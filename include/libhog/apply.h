#ifndef APPLY_H_
#define APPLY_H_

#include "libhog/command.h"
#include "libhog/config.h"
#include "libhog/vec.h"
#include "libhog/buffer.h"

/**
 * Apply a command to an input buffer and write the result
 * to buf. offset is the "starting" byte of the operation
 * returns the new offset
 */
size_t hog_apply(struct hog_config *cfg, struct hog_buffer *buf,
                 const uint8_t *input, size_t len, struct hog_cmd *cmd,
                 size_t offset);

#endif
