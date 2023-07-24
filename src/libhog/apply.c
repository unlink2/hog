#include "libhog/apply.h"
#include "libhog/buffer.h"
#include "libhog/command.h"
#include "libhog/config.h"
#include "libhog/error.h"
#include "libhog/macros.h"
#include "libhog/types.h"
#include "libhog/vec.h"
#include "libhog/log.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

// TODO: this is a non-standard header
// we will have to re-implement this by hand
// for systems not using glibc
#include <endian.h>

#define HOG_BUF_FMT_DEFAULT_LEN 32

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

  int64_t res = 0;
  memcpy(&res, start, read_len);

  if (rc->endianess == HOG_ENDIAN_BIG) {
    switch (read_len) {
    case 1:
      break;
    case 2:
      res = htobe16(res);
      break;
    case 3:
    case 4:
      res = htobe32(res);
      break;
    default:
      res = htobe64(res);
      break;
    }
  } else {
    switch (read_len) {
    case 1:
      break;
    case 2:
      res = htole16(res);
      break;
    case 3:
    case 4:
      res = htole32(res);
      break;
    default:
      res = htole64(res);
      break;
    }
  }

  return res;
}

void hog_apply_fmt_int(struct hog_rc *rc, struct hog_buffer *buf, int64_t data,
                       enum hog_types type) {

  size_t written = 0;
  char *b = (char *)hog_buffer_next(buf, HOG_BUF_FMT_DEFAULT_LEN);

  switch (rc->int_fmt) {
  case HOG_FMT_INT_DEC:
    switch (type) {
    case HOG_TYPE_U8:
      written = sprintf(b, "%d", (uint8_t)data);
      break;
    case HOG_TYPE_I8:
      written = sprintf(b, "%d", (int8_t)data);
      break;
    case HOG_TYPE_U16:
      written = sprintf(b, "%d", (uint16_t)data);
      break;
    case HOG_TYPE_I16:
      written = sprintf(b, "%d", (int16_t)data);
      break;
    case HOG_TYPE_U32:
      written = sprintf(b, "%d", (uint32_t)data);
      break;
    case HOG_TYPE_I32:
      written = sprintf(b, "%d", (int32_t)data);
      break;
    case HOG_TYPE_I64:
    case HOG_TYPE_ISIZE:
      written = sprintf(b, "%ld", (int64_t)data);
      break;
    case HOG_TYPE_U64:
    case HOG_TYPE_USIZE:
    default:
      written = sprintf(b, "%ld", (uint64_t)data);
      break;
    }
    break;
  case HOG_FMT_INT_HEX:
    written = sprintf(b, "0x%lx", data);
    break;
  case HOG_FMT_INT_BIN:
    // FIXME: print 64 bit integers in binary
    written = sprintf(b, "0b%b", (int)data);
    break;
  case HOG_FMT_INT_CHAR:
    written = sprintf(b, "'%c'", (char)data);
    break;
  }

  hog_buffer_adv(buf, written);
}

size_t hog_apply_fmt_type(struct hog_rc *rc, struct hog_buffer *buf,
                          const uint8_t *input, size_t len,
                          const struct hog_type *t, size_t offset) {
  size_t size = hog_type_sizeof(t, rc->cfg->arch_size);
  int64_t data = hog_apply_read(rc, input, len, offset, size);
  if (hog_err()) {
    return 0;
  }

  if (t->ptr_to_idx != HOG_NULL_IDX && t->type != HOG_TYPE_ARRAY &&
      t->type != HOG_TYPE_STRUCT) {
    hog_apply_fmt_int(rc, buf, data, t->type);
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
      hog_apply_fmt_int(rc, buf, data, t->type);
      break;
    case HOG_TYPE_F32: {
      char *c = (char *)hog_buffer_next(buf, HOG_BUF_FMT_DEFAULT_LEN);
      float *d = (float *)&data;
      size_t written = sprintf(c, "%f", *d);
      hog_buffer_adv(buf, written);
    } break;
    case HOG_TYPE_F64: {
      char *c = (char *)hog_buffer_next(buf, HOG_BUF_FMT_DEFAULT_LEN);
      double *d = (double *)&data;
      size_t written = sprintf(c, "%f", *d);
      hog_buffer_adv(buf, written);
    } break;
    case HOG_TYPE_STRUCT: {
      const struct hog_cmd *sc = hog_vec_get(&rc->cfg->cmds, t->struct_cmd_idx);
      if (!sc) {
        hog_err_set(HOG_ERR_CMD_NOT_FOUND);
        return offset;
      }
      // look up comands and start over
      return hog_apply(rc, buf, input, len, sc, offset);
    }
    case HOG_TYPE_ENUM:
      // TODO: implement enums
      break;
    case HOG_TYPE_ARRAY:
      // special case for an array type
      // if it is array call apply the type pointed to n times
      {
        const struct hog_type *tn =
            hog_vec_get(&rc->cfg->types, t->array_type_idx);
        if (!tn) {
          hog_err_set(HOG_ERR_TYPE_NOT_FOUND);
          return offset;
        }
        hog_buffer_fill(buf, rc->cfg->array_open, 1);
        hog_buffer_fill(buf, ' ', 1);
        for (size_t i = 0; i < t->array_cnt; i++) {
          offset = hog_apply_fmt_type(rc, buf, input, len, tn, offset);
          hog_buffer_fill(buf, rc->cfg->array_sep, 1);
          hog_buffer_fill(buf, ' ', 1);
        }
        hog_buffer_fill(buf, rc->cfg->array_close, 1);
      }
      return offset;
    }
  }

  return offset + size;
}

size_t hog_apply_fmt_type_cmd(struct hog_rc *rc, struct hog_buffer *buf,
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
  return hog_apply_fmt_type(rc, buf, input, len, t, offset);
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
    hog_buffer_fill(buf, rc->cfg->scope_open, 1);
    hog_buffer_fill(buf, rc->cfg->new_line, 1);
    rc->scope_level++;
    break;
  case HOG_CMD_END_SCOPE:
    hog_buffer_fill(buf, rc->cfg->new_line, 1);
    hog_buffer_fill(buf, rc->cfg->scope_close, 1);
    hog_buffer_fill(buf, rc->cfg->new_line, 1);
    rc->scope_level--;
    break;
  case HOG_CMD_FMT_TYPE:
    move = hog_apply_fmt_type_cmd(rc, buf, input, len, cmd, offset);
    break;
  case HOG_CMD_FMT_STATIC_LITERAL:
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

  // indent scope
  size_t indent_cnt = (size_t)rc->cfg->indent_cnt * rc->scope_level;
  hog_buffer_fill(buf, rc->cfg->indent_char, indent_cnt);

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
