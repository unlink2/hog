#ifndef MACHINE_H_
#define MACHINE_H_

#include "libhog/config.h"
#include "libhog/io.h"
#include "libhog/macros.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HOG_VM_DEFAULT_MEM_SIZE 1024
#define HOG_VM_DEFAULT_RA_SIZE 128

enum hog_ops {
  // no operation
  HOG_OP_NOP,

  // halts the machine
  // pops 8 bits off the stack and uses it as exit code
  HOG_OP_HLT,

  // waits for input from program
  // input and parses the next instruction
  HOG_OP_PARSE,

  // ouput a string (depends on vm->fmt)
  // takes n bytes from the stack
  // and fromats them according to fmt options
  // if in string mode it will pop an address
  // where the string should be located
  HOG_OP_PUTS,

  // pops a value off the stack and
  // pushes it back twice
  // reads the next 8 bits to allow duping multiple values at a time
  HOG_OP_DUP,

  // integers
  HOG_OP_T8,
  HOG_OP_T16,
  HOG_OP_T32,
  HOG_OP_T64,
  HOG_OP_TWORD,

  HOG_OP_TF, // float
  HOG_OP_TD, // double

  // pops 2 values off the stack, apply operation
  // and push result
  HOG_OP_ADD,
  HOG_OP_SUB,
  HOG_OP_MUL,
  HOG_OP_DIV,
  HOG_OP_BIT_AND,
  HOG_OP_BIT_OR,
  HOG_OP_BIT_XOR,
  HOG_OP_BIT_NOT,
  HOG_OP_AND,
  HOG_OP_OR,

  // equal comparison
  HOG_OP_EQ,
  // greater
  HOG_OP_GT,
  // less
  HOG_OP_LT,
  // pops one value off the stack and nots it
  HOG_OP_NOT,

  // pops word off the stack and jumps
  HOG_OP_JMP,

  // pops value off the stack and jumps
  // if it is 1 by getting a word off the stack
  HOG_OP_JMP_IF,

  // pops a word and jmps
  // pushes the callee's ip+1 to the stack
  HOG_OP_CALL,

  // pops 64 bits off the stack and sets ip
  // to its value
  HOG_OP_RET,

  // reads n bytes (len gotten from stack) from the input stream and
  // pushes them to the stack
  HOG_OP_READ,

  // pops an address from the stack
  // and a value from the stack
  // and reads/writes the value to the address
  HOG_OP_READ_REL,
  HOG_OP_WRITE_REL,

  // push and pop
  HOG_OP_PUSH,
  HOG_OP_POP,

  // lookup a word by name
  // pops a string's address of the stack
  // and pushes a corresponding word onto the stack
  HOG_OP_LOOKUP,

  // register manip
  HOG_OP_PUSH_SP,
  HOG_OP_POP_SP,
  HOG_OP_PUSH_IP,
  HOG_OP_POP_IP,

  // output commands
  // pops n bits off the stack and outputs them

  // set integer format
  HOG_OP_FMT_UDEC, // unsigned
  HOG_OP_FMT_IDEC, // signed
  HOG_OP_FMT_CHAR,
  HOG_OP_FMT_HEX,
  HOG_OP_FMT_BIN,
  HOG_OP_FMT_STR,
  HOG_OP_FMT_F // float

};

struct hog_word_map {
  size_t addr;
  char *word;
};

struct hog_word_map hog_word_map_init(size_t addr, const char *word);

void hog_word_map_free(struct hog_word_map *self);

struct hog_vm {
  int8_t *mem;
  size_t mem_size;

  // call stack
  size_t *ra_stack;
  size_t ra_ptr;
  size_t ra_len;

  struct hog_word_map *words;
  size_t words_len;

  enum hog_ops opt;
  enum hog_ops fmt;

  // used for parser tmp value
  enum hog_ops opt_parser;

  // user facing input
  FILE *stdin;
  // user facing output
  FILE *stdout;
  // fin is the target file buffer
  FILE *fin;

  // registers
  size_t sp; // stack pointer
  size_t ip; // instruction pointer

  bool hlt;
};

struct hog_vm hog_vm_init(size_t mem_size, FILE *stdin, FILE *stdout,
                          FILE *fin);

void hog_vm_def(struct hog_vm *self, size_t addr, const char *word);
void hog_vm_undef(struct hog_vm *self, size_t addr, const char *word);

struct hog_word_map *hog_vm_lookup(struct hog_vm *self, const char *word);

size_t hog_vm_opt_len(enum hog_ops op);

// pop a value from the stack
int8_t hog_vm_pop1(struct hog_vm *self);

// pop n bytes from the stack into *data
size_t hog_vm_popn(struct hog_vm *self, void *data, size_t len);

// push a new value to the stack
int8_t hog_vm_push1(struct hog_vm *self, int8_t data);

// push n bytes to the stack
size_t hog_vm_pushn(struct hog_vm *self, void *data, size_t len);

// reads len into buffer
size_t hog_vm_readn(struct hog_vm *self, size_t src, int8_t *buffer,
                    size_t len);
size_t hog_vm_read1(struct hog_vm *self, size_t src, int8_t *buffer);

// writes buffer of len to mem
size_t hog_vm_writen(struct hog_vm *self, size_t dst, const int8_t *buffer,
                     size_t len);
size_t hog_vm_write1(struct hog_vm *self, size_t dst, const int8_t *buffer);

// interpret the current value at ip as an instruction and execute it
// this will move ip to a new value and change the vm's state
int8_t hog_vm_tick(struct hog_vm *self);
size_t hog_vm_tick_all(struct hog_vm *self);
size_t hog_vm_main(struct hog_vm *self, const char *start_word);

void hog_vm_free(struct hog_vm *self);

#endif
