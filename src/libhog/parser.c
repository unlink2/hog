#include "libhog/parser.h"
#include "libhog/error.h"
#include "libhog/log.h"
#include "libhog/machine.h"
#include <ctype.h>

size_t hog_tok_next(hog_read read, int fd, char *buffer, size_t len) {
  char c = '\0';
  size_t written = 0;
  size_t read_res = 0;
  buffer[0] = '\0';
  while ((read_res = read(fd, &c, 1)) != -1) {
    if (read_res == -1) {
      return read_res;
    }

    // exit conditions
    if ((isspace(c) && written != 0) || c == '\0' || c == '\n' ||
        written >= len - 1) {
      break;
    }

    if (!isspace(c)) {
      buffer[written] = c;
      written++;
    }
  }

  buffer[written] = '\0';

  return written;
}

void hog_parse(struct hog_vm *vm) {
  const size_t buffer_len = 128;
  const char buffer[buffer_len];

  char op = '\0';
  vm->read(vm->stdin, &op, 1);

  switch (op) {
  case ':':
    // TODO: define word at current sp address
    break;
  case 'e':
    // halt command
    hog_vm_push(vm, HOG_OP_HLT);
    break;
  case 'P':
    break;
  case '?':
    // TODO: output syntax help
    break;
  default:
    hog_err_fset(HOG_ERR_PARSE_UNKNOWN_OP, "Op '%c' was not found!\n", op);
    break;
  }
}
