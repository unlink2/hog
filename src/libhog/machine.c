#include "libhog/machine.h"
#include "libhog/error.h"
#include "libhog/log.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct hog_vm hog_vm_init(struct hog_config *cfg) {
  size_t total_size = cfg->mem_size * sizeof(int8_t);

  struct hog_vm self;
  memset(&self, 0, sizeof(self));
  self.mem = malloc(total_size);
  memset(self.mem, 0, total_size);
  self.mem_size = cfg->mem_size;
  self.sp = 0;
  self.ip = 0;

  return self;
}

bool hog_vm_is_in_bounds(size_t max_len, size_t i) { return i <= max_len; }

int8_t hog_vm_pop(struct hog_vm *self) {
  size_t next_sp = 0;
  if (self->sp == 0) {
    hog_err_fset(HOG_ERR_VM_STACK_UNDERFLOW, "[vm] Stack underflow!\n");
    next_sp = self->mem_size;
  } else {
    next_sp = self->sp - 1;
  }

  self->sp = next_sp;
  return self->mem[next_sp];
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

int8_t hog_vm_push(struct hog_vm *self, int8_t data) {
  size_t next_sp = 0;
  if (self->sp >= self->mem_size) {
    size_t prev = self->mem_size;
    hog_vm_resize(self, prev + 1);

    if (prev == self->mem_size) {
      hog_err_fset(HOG_ERR_VM_STACK_OVERFLOW, "[vm] stack overflow!\n");
    }
  }

  next_sp = self->sp + 1;

  self->mem[next_sp] = data;
  self->sp = next_sp;

  return data;
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

int8_t hog_vm_tick(struct hog_vm *self, struct hog_config *cfg) {
  int8_t op = 0;
  self->ip += hog_vm_read1(self, self->ip, &op);

  switch ((enum hog_ops)op) {
  case HOG_OP_NOP:
    break;
  case HOG_OP_HLT:
    self->hlt = true;
    break;
  case HOG_OP_PUTS: {
    char c = '\0';
    while ((c = (char)hog_vm_read1(self, self->ip, (int8_t *)&c))) {
      self->write(self->stdout, (int8_t *)&c, 1);
    }
  } break;
  default:
    hog_err_fset(HOG_ERR_VM_INVAL_OP, "Invalid operation: %d\n", op);
    break;
  }

  return op;
}

void hog_vm_free(struct hog_vm *self) { free(self->mem); }
