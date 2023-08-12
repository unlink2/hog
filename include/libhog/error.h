#ifndef ERROR_H_
#define ERROR_H_

#include "libhog/macros.h"
#include <stddef.h>
#include <errno.h>
#include <stdio.h>

/**
 * Error is a error type struct that provides
 * an errno-style interface.
 */

// Possible error types.
// Some may or may not require special error info
enum hog_error {
  HOG_OK = 0,

  HOG_ERR_VM_STACK_UNDERFLOW,
  HOG_ERR_VM_STACK_OVERFLOW,

  HOG_ERR_VM_INVAL_OP,
  HOG_ERR_VM_MEM_RESIZE_FAILED,
  HOG_ERR_VM_MEM_OOB,

  HOG_ERR_PARSE_UNKNOWN_OP,
  HOG_ERR_PARSE_UNEXPECTED_TOKEN,
  HOG_ERR_PARSE_EXPECTED_STRING,
  HOG_ERR_PARSE_UNTERMINATED_STRING,
  HOG_ERR_PARSE_WORD_NOT_FOUND,
  HOG_ERR_PARSE_UNTERMINATED_CHAR,
  HOG_ERR_INVAL_NUM,

  HOG_ERR_NO_RC,

  // errno is stored in err_detail
  HOG_ERRNO
};

#define hog_errno()                                                            \
  { hog_err_set(HOG_ERRNO); }

#define hog_err_fset(err, ...)                                                 \
  {                                                                            \
    hog_err_set((err));                                                        \
    hog_error(__VA_ARGS__);                                                    \
  }

// sets err to a value
// to indicate a failure in a call
void hog_err_set(enum hog_error err);

// return error code
enum hog_error hog_err(void);

#endif
