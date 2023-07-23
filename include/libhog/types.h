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

  // an array will use the array count field
  HOG_TYPE_ARRAY,

  // an enum is a named list of integer assignments
  HOG_TYPE_ENUM,
};

struct hog_type {
  // TODO: is this name even useful?
  const char *name;
  enum hog_types type;

  // how many elements the array has
  // only ise used when the tpye is TYPE_ARRAY
  size_t array_cnt;

  // index into list of all possible
  // types this item points to
  // if the type is array this pointer
  // is the concrete data type of the array
  size_t ptr_to_idx;
};

struct hog_type hog_type_init(enum hog_types type, const char *name,
                              size_t ptr_to);

int hog_type_ptr_depth(const struct hog_type *self, struct hog_vec *types);

size_t hog_type_sizeof(const struct hog_type *self, size_t arch_len);

void hog_type_free(struct hog_type *self);

void hog_type_vec_free(struct hog_vec *self);

// simple map between a type and its name
// using pointers allows typedef like re-naming while sitll
// referencing essentially the same type internally
struct hog_type_map {
  const char *name;
  size_t type;
};

struct hog_type_map hog_type_map_init(const size_t type, const char *name);

void hog_type_map_free(struct hog_type_map *self);

void hog_type_map_vec_free(struct hog_vec *self);

#endif
