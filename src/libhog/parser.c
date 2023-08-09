#include "libhog/parser.h"
#include "libhog/error.h"
#include "libhog/log.h"
#include "libhog/machine.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>

int hog_fgetc(FILE *i, FILE *tmp) {
  int c = fgetc(i);
  fputc(c, tmp);
  return c;
}

size_t hog_tok_next(FILE *f, char *buffer, size_t len, FILE *tmp) {
  int c = '\0';
  size_t written = 0;
  size_t read_res = 0;
  buffer[0] = '\0';
  while ((c = hog_fgetc(f, tmp)) != -1) {
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

int64_t hog_parse_number(struct hog_vm *vm, size_t len, FILE *tmp) {
  const size_t buffer_len = 128;
  char buffer[buffer_len];

  size_t written = hog_tok_next(vm->stdin, (char *)&buffer, buffer_len, tmp);

  int64_t val = 0;

  char *end = NULL;
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
    double fval = strtof(buffer, &end);
    memcpy(&val, &fval, sizeof(fval));
    if (errno) {
      hog_errno();
      return 0;
    }
  } else {
    // int
    val = strtol(buffer, &end, 0);
    if (errno) {
      hog_errno();
      return 0;
    }
  }

  if (end != NULL && strlen(end) != 0) {
    hog_err_fset(HOG_ERR_INVAL_NUM, "Invalid number %s\n", buffer);
  }

  return val;
}

void hog_parse_fmt(struct hog_vm *vm, FILE *tmp) {
  int op = hog_fgetc(vm->stdin, tmp);

  switch (op) {
  case 's':
    hog_vm_push1(vm, HOG_OP_FMT_STR);
    break;
  case 'i':
    hog_vm_push1(vm, HOG_OP_FMT_UDEC);
    break;
  case 'd':
    hog_vm_push1(vm, HOG_OP_FMT_IDEC);
    break;
  case 'x':
    hog_vm_push1(vm, HOG_OP_FMT_HEX);
    break;
  case 'b':
    hog_vm_push1(vm, HOG_OP_FMT_BIN);
    break;
  case 'f':
    hog_vm_push1(vm, HOG_OP_FMT_F);
    break;
  case 'c':
    hog_vm_push1(vm, HOG_OP_FMT_CHAR);
    break;
  default:
    hog_err_fset(HOG_ERR_PARSE_UNKNOWN_OP, "Op '%c' was not found!\n", op);
    break;
  }
}

// parse a word and push its address
void hog_parse_def_word(struct hog_vm *vm, FILE *tmp) {
  const size_t buf_len = 64;
  char buf[buf_len];
  hog_tok_next(vm->stdin, buf, buf_len, tmp);

  if (hog_err()) {
    return;
  }
  hog_vm_def(vm, vm->sp, buf);
}

// parse a word and undef its address
void hog_parse_undef_word(struct hog_vm *vm, FILE *tmp) {
  const size_t buf_len = 64;
  char buf[buf_len];
  hog_tok_next(vm->stdin, buf, buf_len, tmp);

  if (hog_err()) {
    return;
  }
  hog_vm_undef(vm, vm->sp, buf);
}

size_t hog_parse_word(struct hog_vm *vm, FILE *tmp) {
  const size_t buf_len = 64;
  char buf[buf_len];
  hog_tok_next(vm->stdin, buf, buf_len, tmp);

  if (hog_err()) {
    return 0;
  }

  struct hog_word_map *map = hog_vm_lookup(vm, buf);

  if (!map) {
    hog_err_fset(HOG_ERR_PARSE_WORD_NOT_FOUND, "Word not found: %s\n", buf);
    return 0;
  }

  return map->addr;
}

void hog_parse_from(struct hog_vm *vm, FILE *f) {
  FILE *stdin_before = vm->stdin;
  vm->stdin = f;
  hog_parse_all(vm);
  vm->stdin = stdin_before;
}

void hog_parse_all(struct hog_vm *vm) {
  FILE *tmp = tmpfile();
  while (hog_parse(vm, tmp) != -1 && !hog_err()) {
  }
  fclose(tmp);
}

int hog_parse(struct hog_vm *vm, FILE *tmp) {
  // save original sp so we can revert in case of error
  size_t start_sp = vm->sp;

  int op = hog_fgetc(vm->stdin, tmp);

  while (isspace(op)) {
    op = hog_fgetc(vm->stdin, tmp);
  }

  // filter comments
  if (op == '#') {
    while (op != '\n' && op != '\0' && op != -1) {
      op = hog_fgetc(vm->stdin, tmp);
    }
    return '#';
  }

  switch (op) {
  case ':':
    hog_parse_def_word(vm, tmp);
    break;
  case 'U':
    hog_parse_undef_word(vm, tmp);
    break;
  case 'Y':
    hog_vm_push1(vm, HOG_OP_READ_PTR);
    break;
  case 'W':
    hog_vm_push1(vm, HOG_OP_WRITE_PTR);
    break;
  case 'S': // Stack push
    hog_vm_push1(vm, HOG_OP_PUSH_SP);
    break;
  case 'E': // End scope
    hog_vm_push1(vm, HOG_OP_POP_SP);
    break;
  case 'I': // Instruction ptr push
    hog_vm_push1(vm, HOG_OP_PUSH_IP);
    break;
  case ';':
    hog_vm_push1(vm, HOG_OP_RET);
    break;
  case 'b':
    hog_vm_push1(vm, HOG_OP_T8);
    vm->opt_parser = HOG_OP_T8;
    break;
  case 'A':
    hog_vm_push1(vm, HOG_OP_TSIZE);
    vm->opt_parser = HOG_OP_TSIZE;
    break;
  case 's':
    hog_vm_push1(vm, HOG_OP_T16);
    vm->opt_parser = HOG_OP_T16;
    break;
  case 'i':
    hog_vm_push1(vm, HOG_OP_T32);
    vm->opt_parser = HOG_OP_T32;
    break;
  case 'l':
    hog_vm_push1(vm, HOG_OP_T64);
    vm->opt_parser = HOG_OP_T64;
    break;
  case 'f':
    hog_vm_push1(vm, HOG_OP_TF);
    vm->opt_parser = HOG_OP_TF;
    break;
  case 'd':
    hog_vm_push1(vm, HOG_OP_TD);
    vm->opt_parser = HOG_OP_TD;
    break;
  case 'w':
    hog_vm_push1(vm, HOG_OP_TWORD);
    vm->opt_parser = HOG_OP_TWORD;
    break;
  case 'e':
    // halt command
    hog_vm_push1(vm, HOG_OP_HLT);
    break;
  case 'p': {
    size_t len = hog_vm_opt_len(vm->opt_parser);
    if (hog_err()) {
      break;
    }
    int64_t num = 0;

    if (vm->opt_parser == HOG_OP_TWORD) {
      num = (int64_t)hog_parse_word(vm, tmp);
    } else {
      num = hog_parse_number(vm, len, tmp);
    }

    switch (vm->opt_parser) {
    case HOG_OP_T8: {
      hog_vm_push1(vm, HOG_OP_PUSH);
      int8_t n = (int8_t)num;
      hog_vm_pushn(vm, &n, len);
    } break;
    case HOG_OP_T16: {
      hog_vm_push1(vm, HOG_OP_PUSH);
      int16_t n = (int16_t)num;
      hog_vm_pushn(vm, &n, len);
    } break;
    case HOG_OP_T32: {
      hog_vm_push1(vm, HOG_OP_PUSH);
      int32_t n = (int32_t)num;
      hog_vm_pushn(vm, &n, len);
    } break;
    case HOG_OP_T64: {
      hog_vm_push1(vm, HOG_OP_PUSH);
      int64_t n = (int64_t)num;
      hog_vm_pushn(vm, &n, len);
    } break;
    case HOG_OP_TF: {
      hog_vm_push1(vm, HOG_OP_PUSH);
      double n = 0;
      memcpy(&n, &num, sizeof(n));
      float nf = (float)n;
      hog_vm_pushn(vm, &nf, len);
    } break;
    case HOG_OP_TD: {
      hog_vm_push1(vm, HOG_OP_PUSH);
      double n = 0;
      memcpy(&n, &num, sizeof(n));
      hog_vm_pushn(vm, &n, len);
    } break;
    case HOG_OP_TSIZE:
    case HOG_OP_TWORD: {
      hog_vm_push1(vm, HOG_OP_PUSH);
      size_t n = num;
      hog_vm_pushn(vm, &n, sizeof(n));
    } break;
    default:
      // not possible!
      abort();
    }
  } break;
  case 'P':
    hog_vm_push1(vm, HOG_OP_POP);
    break;
  case 'F':
    hog_vm_push1(vm, HOG_OP_SP_FWD);
    break;
  case 'B':
    hog_vm_push1(vm, HOG_OP_SP_BACK);
    break;
  case 'D':
    hog_vm_push1(vm, HOG_OP_DUP);
    break;
  case 'R':
    hog_vm_push1(vm, HOG_OP_READ);
    break;
  case 'r':
    hog_vm_push1(vm, HOG_OP_RET);
    break;
  case 'c':
    hog_vm_push1(vm, HOG_OP_CALL);
    break;
  case 'j':
    hog_vm_push1(vm, HOG_OP_JMP);
    break;
  case 'J':
    hog_vm_push1(vm, HOG_OP_JMP_IF);
    break;
  case '.':
    hog_vm_push1(vm, HOG_OP_PUTS);
    break;
  case 'O':
    hog_vm_push1(vm, HOG_OP_PUTS_ABS);
    break;
  case '%':
    hog_parse_fmt(vm, tmp);
    break;
  case '!':
    hog_vm_push1(vm, HOG_OP_NOT);
    break;
  case '=':
    hog_vm_push1(vm, HOG_OP_EQ);
    break;
  case '>':
    hog_vm_push1(vm, HOG_OP_LT);
    break;
  case '<':
    hog_vm_push1(vm, HOG_OP_GT);
    break;
  case '"': {
    // has to start with "
    int c = op;
    while ((c = hog_fgetc(vm->stdin, tmp)) != -1 && c != '\0' && c != '"') {
      // handle escaping
      if (c == '\\') {
        c = fgetc(vm->stdin);
        if (c == -1) {
          break;
        }
      }
      hog_vm_push1(vm, (char)c);
    }
    hog_vm_push1(vm, '\0');

    // has to end with "
    if (c != '"') {
      hog_err_fset(HOG_ERR_PARSE_UNTERMINATED_STRING, "Unterminated string\n");
    }
  } break;

  case '+':
    hog_vm_push1(vm, HOG_OP_ADD);
    break;
  case '-':
    hog_vm_push1(vm, HOG_OP_SUB);
    break;
  case '*':
    hog_vm_push1(vm, HOG_OP_MUL);
    break;
  case '/':
    hog_vm_push1(vm, HOG_OP_DIV);
    break;
  case '&':
    hog_vm_push1(vm, HOG_OP_BIT_AND);
    break;
  case '|':
    hog_vm_push1(vm, HOG_OP_BIT_OR);
    break;
  case '~':
    hog_vm_push1(vm, HOG_OP_BIT_NOT);
    break;
  case '^':
    hog_vm_push1(vm, HOG_OP_BIT_XOR);
    break;
  case 'a':
    hog_vm_push1(vm, HOG_OP_AND);
    break;
  case 'o':
    hog_vm_push1(vm, HOG_OP_OR);
    break;
  case 'L':
    hog_vm_push1(vm, HOG_OP_LOOKUP);
    break;
  case ',':
    hog_vm_push1(vm, HOG_OP_PARSE);
    break;
  case 'N':
    hog_vm_push1(vm, HOG_OP_NOP);
    break;
  case '?':
    // TODO: output syntax help
    puts("Command reference...\n");
    break;
  case -1:
    break;
  default:
    hog_err_fset(HOG_ERR_PARSE_UNKNOWN_OP, "Op '%c' was not found!\n", op);
    break;
  }

  if (hog_err()) {
    goto error;
  }

  return op;
error:
  vm->sp = start_sp;
  return 0;
}
