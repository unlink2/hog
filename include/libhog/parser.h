#ifndef PARSER_H_
#define PARSER_H_

#include "libhog/config.h"

// Sayntax:
//
// The syntax is in essence very similar to
// C struct definitions:
// example:
// struct struct_name { u8 val1; u8 val2; u16 val3 };
//
// Keywords:
//  All identifiers starting with 2 __ are reserved!
//  Also assume that all Keywords that are reserved in C are also reserved in
//  this syntax even if they are unused as of now
//
// Identifiers:
//  Identifiers are alphanumeric and may contain underscores
//  they always start with a letter or an undersocre
//
// Built-in data types:
//   u8, u16, u32, u64
//   i8, i16, i32, i64
//   f32, f64
//   char*
//   void, void*
//
// Instructions:
//   There are certain instructions that can be defined alongside
//   types to insert non-type related commands.
//   Those instructions start with a # and end with a new line
//   similar to C's preprocessor directives
//
//   #intfmt hex: Will cause integers to be formatted as hexadecimal numbers
//   #intfmt dec: Will cause integers to be formatted as decimal numbers
//   #intfmt bin: Will cause integers to be formatted as binary numbers
//   #skip <number>: Will instruct the program to skip the next n bytes

enum hog_tok_type {
  HOG_TOK_UNKNOWN,
  HOG_TOK_SEMICOLON,
  HOG_TOK_STRUCT,
  HOG_TOK_IDENT,
  HOG_TOK_DIRECTIVE
};

struct hog_tok {
  enum hog_tok_type type;
  const char *raw;
  size_t raw_len;
};

// returns the next token or NULL if no token is available
size_t hog_parse_next_tok(const char *input);

// parses a single member of a struct
size_t hog_parse_member(struct hog_config *cfg, const char *input);

// takes a textual representation of a struct
// and converts it to commands that are added to
// the command list.
// returns the index of the newly created command list
size_t hog_parse_struct(struct hog_config *cfg, const char *input);

#endif
