#ifndef MACHINE_H_
#define MACHINE_H_

#include "libhog/config.h"
#include "libhog/macros.h"
#include <stddef.h>
#include <stdint.h>

#define HOG_VM_DEFAULT_MEM_SIZE 1024

enum hog_ops {
  // no operation
  HOG_OP_NOP,

  // halts the machine
  // pops 8 bits off the stack and uses it as exit code
  HOG_OP_HLT,

  // waits for input from program
  // input and parses the next instruction
  HOG_OP_PARSE,

  // reads from ip until \0 is read
  HOG_OP_PUTS,

  // pops a value off the stack and
  // pushes it back twice
  // reads the next 8 bits to allow duping multiple values at a time
  HOG_OP_DUP,

  // pops 2 values off the stack, apply operation
  // and push result
  HOG_OP_ADD,
  HOG_OP_SUB,
  HOG_OP_MUL,
  HOG_OP_DIV,
  HOG_OP_BIT_AND,
  HOG_OP_BIT_OR,
  HOG_OP_BIT_XOR,
  HOG_OP_AND,
  HOG_OP_OR,

  // equal comparison
  HOG_OP_EQ,

  // not equal comparison
  HOG_OP_NE,

  // greater
  HOG_OP_GT,
  HOG_OP_GT_EQ,

  // less
  HOG_OP_LT,
  HOG_OP_LT_EQ,

  // pops one value off the stack and nots it
  HOG_OP_BIT_NOT,
  HOG_OP_NOT,

  // reads the next 64 bits after op and jmps
  // unconditionally
  HOG_OP_JMP,

  // pop a value off the stack and jump if...
  HOG_OP_JMP_ZERO,
  HOG_OP_JMP_NOT_ZERO,

  // reads the next 64 bits after op and jmps
  // pushes the callee's ip+1+8 to the stack
  HOG_OP_CALL,

  // pops 64 bits off the stack and sets ip
  // to its value
  HOG_OP_RET,

  // Input commands
  // reads n bits from the input stream and
  // pushes them to the stack
  HOG_OP_READ8,
  HOG_OP_READ16,
  HOG_OP_READ32,
  HOG_OP_READ64,

  // output commands
  // pops n bits off the stack and outputs them

  // char literal
  HOG_OP_FMT_CHAR,

  // set integer format
  HOG_OP_INT_FMT_DEC,
  HOG_OP_INT_FMT_HEX,
  HOG_OP_INT_FMT_BIN,

  // output signed integer
  HOG_OP_FMT_I8,
  HOG_OP_FMT_I16,
  HOG_OP_FMT_I32,
  HOG_OP_FMT_I64,

  // output unsigned integer
  HOG_OP_FMT_U8,
  HOG_OP_FMT_U16,
  HOG_OP_FMT_U32,
  HOG_OP_FMT_U64,

  // output float
  HOG_OP_FMT_F32,
  HOG_OP_FMT_F64
};

struct hog_vm {
  int8_t *mem;
  size_t mem_size;

  // registers
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
