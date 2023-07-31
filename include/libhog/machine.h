#ifndef MACHINE_H_
#define MACHINE_H_

#include "libhog/config.h"
#include <stddef.h>
#include <stdint.h>

#define HOG_VM_DEFAULT_STACK_SIZE 1024

typedef int8_t hog_word;

enum hog_ops { HOG_OP_NOP, HOG_OP_HLT, HOG_OP_PUTS };

struct hog_vm {
  hog_word *stack;
  size_t stack_size;
  size_t sp; // stack pointer
  size_t ip; // instruction pointer
  bool hlt;
};

struct hog_vm hog_vm_init(size_t stack_size);

// pop a value from the stack
hog_word hog_vm_pop(struct hog_vm *self);

// push a new value to the stack
hog_word hog_vm_push(struct hog_vm *self, hog_word data);

// interpret the current value at ip as an instruction and execute it
// this will move ip to a new value and change the vm's state
hog_word hog_vm_tick(struct hog_vm *self, struct hog_config *cfg);

void hog_vm_free(struct hog_vm *self);

#endif
