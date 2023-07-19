#ifndef TYPES_H_
#define TYPES_H_

#include "libhog/macros.h"
#include "libhog/vec.h"

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
  // this is the original name of the type
  // as it was defined
  // this may be different than the mapped name
  // this name may be used to look up more complex struct
  // command sequences
  const char *name;
  enum hog_types type;
};

struct hog_type hog_type_init(enum hog_types type, const char *name);

void hog_type_free(struct hog_type *self);

void hog_type_vec_free(struct hog_vec *self);

// simple map between a type and its name
// using pointers allows typedef like re-naming while sitll
// referencing essentially the same type internally
struct hog_type_map {
  const char *name;
  const struct hog_type *type;

  // 0 means the value is a literal
  // > 0 means the type is a pointer where each level of indrection
  // is a pointer to a pointer and so on
  // a pointer's size depends on arch_size in config
  int ptr_level;
};

struct hog_type_map hog_type_map_init(const struct hog_type *type,
                                      const char *name, int ptr_level);

void hog_type_map_free(struct hog_type_map *self);

void hog_type_map_vec_free(struct hog_vec *self);

#endif
