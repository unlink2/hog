#include "libhog/parser.h"
#include "libhog/error.h"
#include "libhog/log.h"
#include "libhog/machine.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>

size_t hog_tok_next(FILE *f, char *buffer, size_t len) {
  int c = '\0';
  size_t written = 0;
  size_t read_res = 0;
  buffer[0] = '\0';
  while ((c = fgetc(f)) != -1) {
    if (read_res == -1) {
      return read_res;
    }
    if ((isspace(c) && written != 0) || c == '\0' || c == '\n' ||
        written >= len - 1) {
      // exit conditions
      break;
    }

    if (!isspace(c)) {
      buffer[written] = (char)c;
      written++;
    }
  }
  buffer[written] = '\0';

  return written;
}

size_t hog_parse_len_op(struct hog_vm *vm, char *len_op) {
  int op = '\0';
  op = fgetc(vm->stdin);
  *len_op = (char)op;

  switch (op) {
  case 'l':
  case 'd':
    return 8;
  case 'i':
  case 'f':
    return 4;
  case 's':
    return 2;
  case 'b':
    return 1;
  default:
    hog_err_fset(HOG_ERR_PARSE_UNKNOWN_OP, "Length-Op '%c' was not found!\n",
                 op);
    break;
  }

  return 0;
}

int64_t hog_parse_number(struct hog_vm *vm, size_t len) {
  const size_t buffer_len = 128;
  char buffer[buffer_len];

  size_t written = hog_tok_next(vm->stdin, (char *)&buffer, buffer_len);

  int64_t val = 0;

  if (buffer[0] == '\'') {
    // char
    size_t index = 1;
    if (buffer[1] == '\\') {
      index = 2;
    }
    if (buffer[index + 1] != '\'' || written > index + 2) {
      hog_err_fset(HOG_ERR_PARSE_UNTERMINATED_CHAR, "Unterminated char\n");
      return 0;
    }
    val = (int64_t)buffer[index];
  } else if (strstr(buffer, ".") != NULL) {
    // float
    double fval = strtof(buffer, NULL);
    memcpy(&val, &fval, sizeof(fval));
    if (errno) {
      hog_errno();
      return 0;
    }
  } else {
    // int
    val = strtol(buffer, NULL, 0);
    if (errno) {
      hog_errno();
      return 0;
    }
  }

  return val;
}

// parse a word and push its address
size_t hog_parse_word(struct hog_vm *vm) {}

void hog_parse(struct hog_vm *vm) {
  // save original sp so we can revert in case of error
  size_t start_sp = vm->sp;

  int op = fgetc(vm->stdin);

  switch (op) {
  case ':':
    // TODO: define word at current sp address
    break;
  case ';':
    hog_vm_push1(vm, HOG_OP_RET);
    break;
  case 'e':
    // halt command
    hog_vm_push1(vm, HOG_OP_HLT);
    break;
  case 'p': {
    char len_op = '\0';
    size_t len = hog_parse_len_op(vm, &len_op);
    if (hog_err()) {
      goto error;
    }
    int64_t num = hog_parse_number(vm, len);

    switch (len_op) {
    case 'b': {
      int8_t n = (int8_t)num;
      hog_vm_pushn(vm, &n, len);
    } break;
    case 's': {
      int16_t n = (int16_t)num;
      hog_vm_pushn(vm, &n, len);
    } break;
    case 'i': {
      int32_t n = (int32_t)num;
      hog_vm_pushn(vm, &n, len);
    } break;
    case 'l': {
      int64_t n = (int64_t)num;
      hog_vm_pushn(vm, &n, len);
    } break;
    case 'f': {
      double n = 0;
      memcpy(&n, &num, sizeof(n));
      float nf = (float)n;
      hog_vm_pushn(vm, &nf, len);
    } break;
    case 'd': {
      double n = 0;
      memcpy(&n, &num, sizeof(n));
      hog_vm_pushn(vm, &n, len);
    } break;
    default:
      // not possible!
      abort();
    }
  } break;
  case 'P':
    // TODO: pull
    {}
    break;
  case 's':
    // ouput a string
    {
      hog_vm_push1(vm, HOG_OP_PUTS);
      size_t addr = hog_parse_word(vm);
      hog_vm_pushn(vm, &addr, sizeof(addr));
    }
    break;
  case '"': {
    // has to start with "
    int c = op;
    while ((c = fgetc(vm->stdin)) != -1 && c != '\0' && c != '"') {
      // handle escaping
      if (c == '\\') {
        c = fgetc(vm->stdin);
        if (c == -1) {
          break;
        }
      }
      hog_vm_push1(vm, (char)c);
    }

    // has to end with "
    if (c != '"') {
      hog_err_fset(HOG_ERR_PARSE_UNTERMINATED_STRING, "Unterminated string\n");
      goto error;
    }
  } break;
  case '?':
    // TODO: output syntax help
    break;
  default:
    hog_err_fset(HOG_ERR_PARSE_UNKNOWN_OP, "Op '%c' was not found!\n", op);
    goto error;
  }

  return;
error:
  vm->sp = start_sp;
}
