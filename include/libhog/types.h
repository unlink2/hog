#ifndef TYPES_H_
#define TYPES_H_

#include "libhog/macros.h"

enum hog_types {
  // 0-sized type
  HOG_TYPE_VOID,

  // int types
  HOG_TYPE_U8,
  HOG_TYPE_U16,
  HOG_TYPE_U32,
  HOG_TYPE_U64,

  // size depends on the arch_size in config
  HOG_TYPE_USIZE,

  HOG_TYPE_I8,
  HOG_TYPE_I16,
  HOG_TYPE_I32,
  HOG_TYPE_I64,

  // size depends on the arch_size in config
  HOG_TYPE_ISIZE,

  // float types
  HOG_TYPE_F32,
  HOG_TYPE_F64,

  // "complex" types

  // a struct is simply another named collection of commands
  // that can be looked up
  HOG_TYPE_STRUCT,

  // an enum is a named list of integer assignments
  HOG_TYPE_ENUM,
};

#define HOG_TYPE_NO_INDIRECTION 0

struct hog_type {
  enum hog_types type;
  const char *name;

  // 0 means the value is a literal
  // > 0 means the type is a pointer where each level of indrection
  // is a pointer to a pointer and so on
  // a pointer's size depends on arch_size in config
  int ptr_level;
};

// simple map between a type and its name
// using pointers allows typedef like re-naming while sitll
// referencing essentially the same type internally
struct hog_type_map {
  const char *name;
  const struct hog_type *type;
};

#endif
