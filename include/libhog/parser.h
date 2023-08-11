#ifndef PARSER_H_
#define PARSER_H_

#include <stddef.h>
#include "libhog/machine.h"

size_t hog_tok_next(FILE *f, char *buffer, size_t len, FILE *tmp);

// parse from file
void hog_parse_from(struct hog_vm *vm, FILE *f);

// eval a command
void hog_parse_eval(struct hog_vm *vm, const char *eval);

// compile all commands until eof
void hog_parse_all(struct hog_vm *vm);

// parse a command skipping white spaces
// will compile the input into bytecode and place it into the vm
// by pushing the result to the vm's stack
int hog_parse(struct hog_vm *vm, FILE *tmp);

#endif
