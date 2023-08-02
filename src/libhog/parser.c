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
    if ((isspace(c) && written != 0) || c == '\0' || c == '\n' ||
        written >= len - 1) {
      // exit conditions
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

size_t hog_parse_len_op(struct hog_vm *vm) {
  char op = '\0';
  vm->read(vm->stdin, &op, 1);

  switch (op) {
  case 'l':
    return 8;
  case 'i':
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

int64_t hog_parse_number(struct hog_vm *vm) {}

// parse a word and push its address
size_t hog_parse_word(struct hog_vm *vm) {}

void hog_parse(struct hog_vm *vm) {
  // save original sp so we can revert in case of error
  size_t start_sp = vm->sp;

  const size_t buffer_len = 128;
  char buffer[buffer_len];

  char op = '\0';
  vm->read(vm->stdin, &op, 1);

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
  case 'p':
    // TODO: push
    {}
    break;
  case 'P':
    // TODO: pull
    {}
    break;
  case 's':
    // ouput a string
    {
      hog_vm_push1(vm, HOG_OP_PUTS);
      hog_vm_push1(vm, hog_parse_word(vm));
    }
    break;
  case '"': {
    // has to start with "
    char c = '\0';
    vm->read(vm->stdin, &c, 1);
    if (c != '"') {
      hog_err_fset(HOG_ERR_PARSE_EXPECTED_STRING, "String expected");
      goto error;
    }

    char prev = c;
    while ((vm->read(vm->stdin, &c, 1) != -1) && c != '\0' &&
           (c != '"' && prev != '\\')) {
      // handle escaping
      if (prev == '\\' || c != '\\') {
        hog_vm_push1(vm, c);
      }

      prev = c;
    }

    // has to end with "
    if (c != '"') {
      hog_err_fset(HOG_ERR_PARSE_UNTERMINATED_STRING, "Unterminated string");
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
