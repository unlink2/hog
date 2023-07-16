#include "libhog/apply.h"
#include "libhog/buffer.h"
#include "libhog/error.h"

size_t hog_apply(struct hog_config *cfg, struct hog_buffer *buf,
                 const uint8_t *input, size_t len, struct hog_cmd *cmd,
                 size_t offset) {
  size_t move = offset;

  switch (cmd->type) {
  case HOG_CMD_INVAL:
    hog_err_set(HOG_ERR_CMD_INVAL);
    break;
  case HOG_CMD_MOVE_BYTES:
    move += cmd->move_bytes;
    break;
  case HOG_CMD_BEGIN:
    hog_buffer_clear(buf);
    break;
  case HOG_CMD_END:
    hog_buffer_null_term(buf);
    break;
  }

  return move;
}
