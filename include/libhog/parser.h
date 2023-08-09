#ifndef PARSER_H_
#define PARSER_H_

#include <stddef.h>
#include "libhog/machine.h"

size_t hog_tok_next(FILE *f, char *buffer, size_t len, FILE *tmp);

void hog_parse_all(struct hog_vm *vm);

// reads from vm->stdin until \n is read
// will compile the input into bytecode and place it into the vm
// by pushing the result to the vm's stack
int hog_parse(struct hog_vm *vm, FILE *tmp);

#endif
