#include "libhog/machine.h"
#include "libhog/error.h"
#include "libhog/log.h"
#include "libhog/color.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct hog_word_map hog_word_map_init(size_t addr, const char *word) {
  struct hog_word_map self;
  memset(&self, 0, sizeof(self));
  self.addr = addr;
  self.word = strdup(word);
  return self;
}

void hog_word_map_free(struct hog_word_map *self) { free((void *)self->word); }

struct hog_vm hog_vm_init(size_t mem_size, FILE *stdin, FILE *stdout,
                          FILE *fin) {
  size_t total_size = mem_size * sizeof(int8_t);

  struct hog_vm self;
  memset(&self, 0, sizeof(self));
  self.mem = malloc(total_size);
  memset(self.mem, 0, total_size);
  self.mem_size = mem_size;
  self.sp = 0;
  self.ip = 0;

  self.opt = HOG_OP_T8;
  self.opt_parser = HOG_OP_T8;
  self.fmt = HOG_OP_FMT_IDEC;

  self.stdin = stdin;
  self.stdout = stdout;
  self.fin = fin;

  self.ra_len = HOG_VM_DEFAULT_RA_SIZE;
  self.ra_ptr = 0;
  self.ra_stack = malloc(self.ra_len * sizeof(size_t));
  memset(self.ra_stack, 0, sizeof(size_t) * self.ra_len);

  return self;
}

void hog_vm_def(struct hog_vm *self, size_t addr, const char *word) {
  // TODO: avoid realloc every call...
  size_t new_len = self->words_len + 1;
  struct hog_word_map *new =
      realloc(self->words, new_len * sizeof(struct hog_word_map));

  if (!new) {
    hog_error("Failed to resize word list\n");
    hog_errno();
    return;
  }

  self->words_len = new_len;
  self->words = new;
  self->words[new_len - 1] = hog_word_map_init(addr, word);
}

void hog_vm_undef(struct hog_vm *self, size_t addr, const char *word) {
  // TODO: actually remove undefed words
  struct hog_word_map *map = hog_vm_lookup(self, word);
  if (map) {
    map->word[0] = '\0';
  }
}

struct hog_word_map *hog_vm_lookup(struct hog_vm *self, const char *word) {
  for (size_t i = 0; i < self->words_len; i++) {
    if (strcmp(word, self->words[i].word) == 0) {
      return &self->words[i];
    }
  }

  return NULL;
}

size_t hog_vm_opt_len(enum hog_ops op) {
  switch (op) {
  case HOG_OP_T8:
    return 1;
  case HOG_OP_T16:
    return 2;
  case HOG_OP_TF:
  case HOG_OP_T32:
    return 4;
  case HOG_OP_TD:
  case HOG_OP_T64:
  case HOG_OP_TWORD:
    return 8;
  default:
    // should never be called with any other value!
    abort();
  }

  return 0;
}

bool hog_vm_is_in_bounds(size_t max_len, size_t i) { return i <= max_len; }

int8_t hog_vm_pop1(struct hog_vm *self) {
  size_t next_sp = 0;
  if (self->sp == 0) {
    hog_err_fset(HOG_ERR_VM_STACK_UNDERFLOW, "[vm] Stack underflow!\n");
    next_sp = self->mem_size;
  } else {
    next_sp = self->sp - 1;
  }

  int8_t val = self->mem[self->sp - 1];
  self->sp = next_sp;
  return val;
}

size_t hog_vm_popn(struct hog_vm *self, void *data, size_t len) {
  size_t total = 0;
  int8_t *d = data;

  for (size_t i = len; i > 0; i--, total++) {
    d[i - 1] = hog_vm_pop1(self);
    if (hog_err()) {
      break;
    }
  }

  return total;
}

int64_t hog_vm_popt(struct hog_vm *self) {
  switch (self->opt) {
  case HOG_OP_T8: {
    int8_t b = 0;
    hog_vm_popn(self, &b, sizeof(b));
    return b;
  }
  case HOG_OP_T16: {
    int16_t b = 0;
    hog_vm_popn(self, &b, sizeof(b));
    return b;
  }
  case HOG_OP_TF:
  case HOG_OP_T32: {
    int32_t b = 0;
    hog_vm_popn(self, &b, sizeof(b));
    return b;
  }
  case HOG_OP_TWORD:
  case HOG_OP_TD:
  case HOG_OP_T64: {
    int64_t b = 0;
    hog_vm_popn(self, &b, sizeof(b));
    return b;
  }
  default:
    hog_error("Invalid puts\n");
    abort();
  }
  return 0;
}

void hog_vm_resize(struct hog_vm *self, size_t target) {
  // attempt to resize memory, and error out if it is not possible
  size_t new_mem_size = MAX(self->mem_size * 2, target);
  int8_t *new_mem = realloc(self->mem, new_mem_size);
  if (!new_mem) {
    hog_err_fset(HOG_ERR_VM_MEM_RESIZE_FAILED, "[vm] mem resize failed!\n");
  } else {
    self->mem = new_mem;
    self->mem_size = new_mem_size;
  }
}

int8_t hog_vm_push1(struct hog_vm *self, int8_t data) {
  size_t next_sp = 0;
  if (self->sp >= self->mem_size) {
    size_t prev = self->mem_size;
    hog_vm_resize(self, prev + 1);

    if (prev == self->mem_size) {
      hog_err_fset(HOG_ERR_VM_STACK_OVERFLOW, "[vm] stack overflow!\n");
    }
  }

  next_sp = self->sp + 1;

  self->mem[self->sp] = data;
  self->sp = next_sp;

  return data;
}

size_t hog_vm_pushn(struct hog_vm *self, void *data, size_t len) {
  size_t i = 0;
  int8_t *d = data;

  for (; i < len; i++) {
    hog_vm_push1(self, d[i]);
    if (hog_err()) {
      break;
    }
  }

  return i;
}

void hog_vm_pusht(struct hog_vm *self, void *b) {
  size_t len = hog_vm_opt_len(self->opt);
  hog_vm_pushn(self, &b, len);
}

size_t hog_vm_readn(struct hog_vm *self, size_t src, int8_t *buffer,
                    size_t len) {
  size_t i = 0;
  for (; i < len; i++) {
    size_t offset = src + i;
    if (!hog_vm_is_in_bounds(self->mem_size, offset)) {
      hog_err_fset(HOG_ERR_VM_MEM_OOB,
                   "Read at %lx is out of bounds! Max mem size is %lx\n",
                   offset, self->mem_size);
      return i;
    }
    buffer[i] = self->mem[offset];
  }

  return i;
}

size_t hog_vm_read1(struct hog_vm *self, size_t src, int8_t *buffer) {
  size_t i = 0;
  size_t offset = src + i;
  if (!hog_vm_is_in_bounds(self->mem_size, offset)) {
    hog_err_fset(HOG_ERR_VM_MEM_OOB,
                 "Read at %lx is out of bounds! Max mem size is %lx\n", offset,
                 self->mem_size);
    return 0;
  }
  buffer[i] = self->mem[offset];

  return 1;
}

size_t hog_vm_writen(struct hog_vm *self, size_t dst, const int8_t *buffer,
                     size_t len) {
  size_t i = 0;

  for (; i < len; i++) {
    size_t offset = dst + i;
    if (!hog_vm_is_in_bounds(self->mem_size, offset)) {
      hog_err_fset(HOG_ERR_VM_MEM_OOB,
                   "Write at %lx is out of bounds! Max mem size is %lx\n",
                   offset, self->mem_size);
      return i;
    }
    self->mem[offset] = buffer[i];
  }

  return i;
}

size_t hog_vm_write1(struct hog_vm *self, size_t dst, const int8_t *buffer) {
  size_t i = 0;
  size_t offset = dst + i;
  if (!hog_vm_is_in_bounds(self->mem_size, offset)) {
    hog_err_fset(HOG_ERR_VM_MEM_OOB,
                 "Write at %lx is out of bounds! Max mem size is %lx\n", offset,
                 self->mem_size);
    return 0;
  }
  self->mem[offset] = buffer[i];

  return 1;
}

void hog_vm_puts_int(struct hog_vm *self, int64_t val) {}

void hog_vm_puts(struct hog_vm *self) {
  size_t len = hog_vm_opt_len(self->opt);
  uint64_t val = hog_vm_popt(self);

  // remove bits we do not want
  uint64_t diff = (sizeof(uint64_t) * 8 - len * 8);
  val = (val << diff >> diff);

  void *val_ptr = &val;
  float *fptr = val_ptr;
  double *dptr = val_ptr;

  switch (self->fmt) {
  case HOG_OP_FMT_HEX:
    fprintf(self->stdout, "%lx", val);
    break;
  case HOG_OP_FMT_BIN:
    fprintf(self->stdout, "%b", (int)val);
    break;
  case HOG_OP_FMT_IDEC:
    fprintf(self->stdout, "%ld", val);
    break;
  case HOG_OP_FMT_UDEC:
    fprintf(self->stdout, "%li", val);
    break;
  case HOG_OP_FMT_F:
    if (len == 8) {
      fprintf(self->stdout, "%f", *dptr);
    } else {
      fprintf(self->stdout, "%f", *fptr);
    }
    break;
  case HOG_OP_FMT_STR:
    // TODO: string output
    hog_warn("String fmt is not implemented!");
    break;
  default:
    hog_error("Invalid puts fmt\n");
    abort();
  }
}

void hog_vm_dbg_dump(struct hog_vm *self) {
  size_t start = 0;
  const size_t range = 0x100;
  if (self->ip > range) {
    start = self->ip - range;
  }

  fprintf(stderr, "Memory dump around ip %lx\n", self->ip);
  for (size_t i = start; i < MIN(start + range, self->mem_size); i++) {
    if (i % 16 == 0) {
      fprintf(stderr, "\n%08lx\t", i);
    }

    if (i == self->ip) {
      hog_term_escape(stderr, HOG_ANSI_COLOR_RED);
    } else if (i == self->sp) {
      hog_term_escape(stderr, HOG_ANSI_COLOR_CYAN);
    }

    fprintf(stderr, "%02x ", self->mem[i]);
    hog_term_escape(stderr, HOG_ANSI_COLOR_RESET);
  }
  fputs("\n", stderr);
}

void hog_vm_push(struct hog_vm *self) {
  size_t len = hog_vm_opt_len(self->opt);
  hog_vm_pushn(self, self->mem + self->ip, len);
  self->ip += len;
}

void hog_vm_pop(struct hog_vm *self) {
  size_t len = hog_vm_opt_len(self->opt);
  int64_t buf = 0;
  hog_vm_popn(self, &buf, len);
}

void hog_vm_dup(struct hog_vm *self) {
  size_t len = hog_vm_opt_len(self->opt);
  int64_t buf = 0;
  hog_vm_popn(self, &buf, len);
  hog_vm_pushn(self, &buf, len);
  hog_vm_pushn(self, &buf, len);
}

int8_t hog_vm_tick(struct hog_vm *self) {
  int8_t op = 0;
  self->ip += hog_vm_read1(self, self->ip, &op);

  switch ((enum hog_ops)op) {
  case HOG_OP_NOP:
    break;
  case HOG_OP_HLT:
    self->hlt = true;
    break;
  case HOG_OP_T8:
  case HOG_OP_T16:
  case HOG_OP_T32:
  case HOG_OP_T64:
  case HOG_OP_TF:
  case HOG_OP_TD:
  case HOG_OP_TWORD:
    self->opt = (int)op;
    break;
  case HOG_OP_FMT_IDEC:
  case HOG_OP_FMT_UDEC:
  case HOG_OP_FMT_HEX:
  case HOG_OP_FMT_BIN:
  case HOG_OP_FMT_F:
  case HOG_OP_FMT_STR:
    self->fmt = (int)op;
    break;
  case HOG_OP_PUSH:
    hog_vm_push(self);
    break;
  case HOG_OP_POP:
    hog_vm_pop(self);
    break;
  case HOG_OP_PUTS:
    hog_vm_puts(self);
    break;
  case HOG_OP_DUP:
    hog_vm_dup(self);
    break;
  case HOG_OP_JMP_IF: {
    int64_t val = hog_vm_popt(self);
    if (!val) {
      break;
    }
  }
  case HOG_OP_POP_IP:
  case HOG_OP_JMP: {
    size_t target = 0;
    hog_vm_popn(self, &target, sizeof(target));
    self->ip = target;
  } break;
  case HOG_OP_PUSH_IP:
    hog_vm_pushn(self, &self->ip, sizeof(self->ip));
    break;
  case HOG_OP_PUSH_SP:
    hog_vm_pushn(self, &self->sp, sizeof(self->sp));
    break;
  case HOG_OP_POP_SP: {
    size_t target = 0;
    hog_vm_popn(self, &target, sizeof(target));
    self->sp = target;
  } break;
  case HOG_OP_CALL: {
    size_t target = 0;
    hog_vm_popn(self, &target, sizeof(target));
    self->ra_stack[self->ra_ptr] = self->ip;
    self->ra_ptr = (self->ra_ptr + 1) % self->ra_len;
    self->ip = target;
  } break;
  case HOG_OP_RET: {
    if (self->ra_ptr == 0) {
      self->ra_ptr = self->ra_len - 1;
    } else {
      self->ra_ptr--;
    }
    self->ip = self->ra_stack[self->ra_ptr];
  } break;
  case HOG_OP_LOOKUP: {
    size_t offset = 0;
    hog_vm_popn(self, &offset, sizeof(offset));
    if (offset >= self->mem_size) {
      hog_err_fset(HOG_ERR_VM_MEM_OOB, "%lx is out of bounds\n", offset);
      break;
    }
    struct hog_word_map *target =
        hog_vm_lookup(self, (const char *)self->mem + offset);
    if (!target) {
      hog_err_fset(HOG_ERR_PARSE_WORD_NOT_FOUND, "Word not found: %s\n",
                   self->mem + offset);
      break;
    }
    hog_vm_pushn(self, &target->addr, sizeof(size_t));
  } break;
  case HOG_OP_ADD: {
    int64_t l = hog_vm_popt(self);
    int64_t r = hog_vm_popt(self);
    int64_t v = l + r;
    hog_vm_pusht(self, &v);
    break;
  }
  default:
    hog_err_fset(HOG_ERR_VM_INVAL_OP, "Invalid operation at %lx: %x\n",
                 self->ip - 1, op);
    hog_vm_dbg_dump(self);
    break;
  }

  return op;
}

size_t hog_vm_tick_all(struct hog_vm *self) {
  size_t ticks = 0;
  while (!self->hlt && !hog_err()) {
    hog_vm_tick(self);
    ticks++;
  }

  return ticks;
}

size_t hog_vm_main(struct hog_vm *self, const char *start_word) {
  if (start_word) {
    struct hog_word_map *w = hog_vm_lookup(self, start_word);
    if (!w) {
      hog_vm_dbg_dump(self);
      hog_err_fset(HOG_ERR_PARSE_WORD_NOT_FOUND, "Word not found %s\n",
                   start_word);
      return 0;
    }
    self->ip = w->addr;
  }
  return hog_vm_tick_all(self);
}

void hog_vm_free(struct hog_vm *self) {
  for (size_t i = 0; i < self->words_len; i++) {
    hog_word_map_free(&self->words[i]);
  }
  if (self->words) {
    free(self->words);
  }

  free(self->mem);
  free(self->ra_stack);
}
