#ifndef TYPES_H_
#define TYPES_H_

#include "libhog/macros.h"

enum hog_types {
  HOG_TYPE_VOID,

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

  HOG_TYPE_STRUCT,
  HOG_TYPE_ENUM,
};

struct hog_type {
  enum hog_types type;

  // size depends on the arch_size in config
  bool ptr;
};

#endif
