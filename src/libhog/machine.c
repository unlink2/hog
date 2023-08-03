#include "libhog/machine.h"
#include "libhog/error.h"
#include "libhog/log.h"
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

  self.stdin = stdin;
  self.stdout = stdout;
  self.fin = fin;

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

void hog_vm_puts_int(struct hog_vm *self, int64_t val) {
  switch (self->fmt) {
  case HOG_OP_FMT_HEX:
    fprintf(self->stdout, "%lx", val);
    break;
  default:
    abort();
  }
}

void hog_vm_puts(struct hog_vm *self) {
  switch (self->opt) {
  case HOG_OP_T8: {
    int8_t b = 0;
    hog_vm_popn(self, &b, sizeof(b));
    hog_vm_puts_int(self, b);
  } break;
  default:
    abort();
  }
}

void hog_vm_dbg_dump(struct hog_vm *self) {
  size_t start = 0;
  const size_t range = 0x100;
  if (self->ip > range) {
    start = self->ip - range;
  }

  printf("Memory dump around ip %lx\n", self->ip);
  for (size_t i = start; i < MIN(start + range, self->mem_size); i++) {
    if (i % 16 == 0) {
      printf("\n%08lx\t", i);
    }
    printf("%02x ", self->mem[i]);
  }
  puts("\n");
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
  default:
    hog_err_fset(HOG_ERR_VM_INVAL_OP, "Invalid operation at %lx: %d\n",
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

void hog_vm_free(struct hog_vm *self) {
  for (size_t i = 0; i < self->words_len; i++) {
    hog_word_map_free(&self->words[i]);
  }
  if (self->words) {
    free(self->words);
  }

  free(self->mem);
}
