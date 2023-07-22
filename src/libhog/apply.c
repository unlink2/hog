#include "libhog/apply.h"
#include "libhog/buffer.h"
#include "libhog/command.h"
#include "libhog/config.h"
#include "libhog/error.h"
#include "libhog/macros.h"
#include "libhog/types.h"
#include "libhog/vec.h"
#include "libhog/log.h"
#include <stdint.h>
#include <string.h>

// TODO: this is a non-standard header
// we will have to re-implement this by hand
// for systems not using glibc
#include <endian.h>

struct hog_rc hog_rc_init(const struct hog_config *cfg) {
  struct hog_rc self;
  memset(&self, 0, sizeof(self));
  self.cfg = cfg;
  self.name = NULL;

  return self;
}

void hog_rc_name(struct hog_rc *rc, const char *name) {
  if (rc->name) {
    free(rc->name);
  }
  rc->name = strdup(name);
}

void hog_rc_free(struct hog_rc *rc) {
  if (rc->name) {
    free(rc->name);
  }
}

int64_t hog_apply_read(struct hog_rc *rc, const uint8_t *input,
                       size_t input_len, size_t offset, size_t read_len) {
  const uint8_t *start = input + offset;
  size_t start_len = input_len - offset;

  if (start_len < read_len) {
    hog_err_set(HOG_ERR_OUT_OF_DATA);
    hog_err_detail(&offset, sizeof(offset));
    return 0;
  }

  uint64_t res = 0;
  memcpy(&res, start, read_len);

  if (rc->endianess == HOG_ENDIAN_BIG) {
    res = htobe64(res);
  } else {
    res = htole64(res);
  }

  return (int64_t)res;
}

void hog_apply_fmt_int(struct hog_rc *rc, struct hog_buffer *buf,
                       int64_t data) {

  size_t written = 0;
  char *b = (char *)hog_buffer_next(buf, 16);

  switch (rc->int_fmt) {
  case HOG_FMT_INT_DEC:
    written = sprintf(b, "%ld", data);
  case HOG_FMT_INT_HEX:
    written = sprintf(b, "0x%lx", data);
  case HOG_FMT_INT_BIN:
    // FIXME: print 64 bit integers in binary
    written = sprintf(b, "0b%b", (int)data);
  case HOG_FMT_INT_CHAR:
    written = sprintf(b, "'%c'", (char)data);
    break;
  }

  hog_buffer_adv(buf, written);
}

size_t hog_apply_fmt_type(struct hog_rc *rc, struct hog_buffer *buf,
                          const uint8_t *input, size_t len,
                          const struct hog_cmd *cmd, size_t offset) {
  // look up data type
  const struct hog_type *t =
      hog_config_type_lookup(rc->cfg, cmd->type_name, NULL);
  if (!t) {
    hog_error("Type '%s' was not found", cmd->type_name);
    hog_err_set(HOG_ERR_TYPE_NOT_FOUND);
    hog_err_detail(cmd->type_name, sizeof(char *));
    return 0;
  }

  size_t size = hog_type_sizeof(t, rc->cfg->arch_size);
  int64_t data = hog_apply_read(rc, input, len, offset, size);
  if (hog_err()) {
    return 0;
  }

  if (t->ptr_to_idx != HOG_NULL_IDX) {
    hog_apply_fmt_int(rc, buf, data);
  } else {
    switch (t->type) {
    case HOG_TYPE_VOID:
      break;
    case HOG_TYPE_U8:
    case HOG_TYPE_U16:
    case HOG_TYPE_U32:
    case HOG_TYPE_U64:
    case HOG_TYPE_USIZE:
    case HOG_TYPE_I8:
    case HOG_TYPE_I16:
    case HOG_TYPE_I32:
    case HOG_TYPE_I64:
    case HOG_TYPE_ISIZE:
      hog_apply_fmt_int(rc, buf, data);
      break;
    case HOG_TYPE_F32:

    case HOG_TYPE_F64:
      break;
    case HOG_TYPE_STRUCT:
      // TODO: implement struct
    case HOG_TYPE_ENUM:
      // TODO: implement enums
      break;
    }
  }

  return offset + size;
}

size_t hog_apply_fmt_literal(struct hog_rc *rc, struct hog_buffer *buf,
                             const uint8_t *input, size_t len,
                             const struct hog_cmd *cmd, size_t offset) {
  if (!cmd->literal) {
    return 0;
  }
  size_t lit_len = strlen(cmd->literal);

  char *b = (char *)hog_buffer_next(buf, lit_len);
  memcpy(b, cmd->literal, lit_len);
  hog_buffer_adv(buf, lit_len);

  return 0;
}

size_t hog_apply_fmt_name(struct hog_rc *rc, struct hog_buffer *buf) {
  if (!rc->name) {
    return 0;
  }
  size_t name_len = strlen(rc->name);

  char *b = (char *)hog_buffer_next(buf, name_len);
  memcpy(b, rc->name, name_len);
  hog_buffer_adv(buf, name_len);

  return 0;
}

size_t hog_apply_next(struct hog_rc *rc, struct hog_buffer *buf,
                      const uint8_t *input, size_t len,
                      const struct hog_cmd *cmd, size_t offset) {
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
  case HOG_CMD_BEGIN_SCOPE:
    rc->scope_level++;
    break;
  case HOG_CMD_END_SCOPE:
    rc->scope_level--;
    break;
  case HOG_CMD_FMT_TYPE:
    move = hog_apply_fmt_type(rc, buf, input, len, cmd, offset);
    break;
  case HOG_CMD_FMT_LITERAL:
    move = hog_apply_fmt_literal(rc, buf, input, len, cmd, offset);
    break;
  case HOG_CMD_FMT_NAME:
    move = hog_apply_fmt_name(rc, buf);
    break;
  }

  return move;
}

size_t hog_apply(struct hog_rc *rc, struct hog_buffer *buf,
                 const uint8_t *input, size_t len, const struct hog_cmd *cmd,
                 size_t offset) {

  const struct hog_vec *cmds = &rc->cfg->cmds;

  // TODO: surely there is a better way to write this loop,
  // but I literally can't think of one right now...
  while (true) {
    offset += hog_apply_next(rc, buf, input, len, cmd, offset);

    if (cmd->next == HOG_NULL_IDX || hog_err()) {
      break;
    }
    cmd = hog_vec_get(cmds, cmd->next);
  }

  return offset;
}

size_t hog_apply_lookup(struct hog_rc *rc, struct hog_buffer *buf,
                        const uint8_t *input, size_t len, const char *cmd_name,
                        size_t offset) {
  const struct hog_cmd *cmd = hog_conifg_cmd_lookup(rc->cfg, cmd_name);
  if (!cmd) {
    hog_err_set(HOG_ERR_CMD_NOT_FOUND);
    hog_err_detail(cmd_name, sizeof(char *));
    return 0;
  }

  size_t res = hog_apply(rc, buf, input, len, cmd, offset);

  hog_buffer_null_term(buf);
  return res;
}
