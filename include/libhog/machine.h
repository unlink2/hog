#ifndef MACHINE_H_
#define MACHINE_H_

#include "libhog/config.h"
#include <stddef.h>
#include <stdint.h>

#define HOG_VM_DEFAULT_MEM_SIZE 1024

enum hog_ops {
  HOG_OP_NOP,
  HOG_OP_HLT,

  // reads from ip until \0 is read
  HOG_OP_PUTS
};

struct hog_vm {
  int8_t *mem;
  size_t mem_size;
  size_t sp; // stack pointer
  size_t ip; // instruction pointer
  bool hlt;
};

struct hog_vm hog_vm_init(size_t stack_size);

// pop a value from the stack
int8_t hog_vm_pop(struct hog_vm *self);

// push a new value to the stack
int8_t hog_vm_push(struct hog_vm *self, int8_t data);

// reads len into buffer
size_t hog_vm_read(struct hog_vm *self, int8_t *buffer, size_t len);

// writes buffer of len to mem
size_t hog_vm_write(struct hog_vm *self, size_t dst, int8_t *buffer,
                    size_t len);

// interpret the current value at ip as an instruction and execute it
// this will move ip to a new value and change the vm's state
int8_t hog_vm_tick(struct hog_vm *self, struct hog_config *cfg);

void hog_vm_free(struct hog_vm *self);

#endif
