#include "libhog/machine.h"
#include "libhog/error.h"
#include "libhog/log.h"
#include <stdlib.h>
#include <string.h>

struct hog_vm hog_vm_init(size_t stack_size) {
  size_t total_size = stack_size * sizeof(hog_word);

  struct hog_vm self;
  memset(&self, 0, sizeof(self));
  self.stack = malloc(total_size);
  memset(self.stack, 0, total_size);
  self.stack_size = stack_size;

  return self;
}

hog_word hog_vm_pop(struct hog_vm *self) {
  size_t next_sp = 0;
  if (self->sp == 0) {
    hog_err_fset(HOG_ERR_VM_STACK_UNDERFLOW, "[vm] Stack underflow!\n");
    next_sp = self->stack_size;
  } else {
    next_sp = self->sp - 1;
  }

  self->sp = next_sp;
  return self->stack[next_sp];
}

hog_word hog_vm_push(struct hog_vm *self, int64_t data) {
  size_t next_sp = 0;
  if (self->sp >= self->stack_size) {
    hog_err_fset(HOG_ERR_VM_STACK_OVERFLOW, "[vm] stack overflow!\n");
    next_sp = 0;
  } else {
    next_sp = self->sp + 1;
  }

  self->stack[next_sp] = data;
  self->sp = next_sp;

  return data;
}

hog_word hog_vm_tick(struct hog_vm *self, struct hog_config *cfg) {
  hog_word op = hog_vm_pop(self);

  switch ((enum hog_ops)op) {
  HOG_OP_NOP:
    break;
  HOG_OP_HLT:
    self->hlt = true;
    break;
  HOG_OP_PUTS:
    break;
  default:
    hog_err_fset(HOG_ERR_VM_INVAL_OP, "Invalid operation: %ld\n", op);
    break;
  }

  return op;
}

void hog_vm_free(struct hog_vm *self) { free(self->stack); }
