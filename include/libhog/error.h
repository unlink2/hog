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
  HOG_ERR_UNTERMINATED_TOKEN,
  HOG_ERR_INI_SECTION_INVAL,
  HOG_ERR_INI_BAD_KEY,
  HOG_ERR_CMD_INVAL,

  HOG_ERR_TYPE_NOT_FOUND,
  // size_t offset as error detail
  HOG_ERR_OUT_OF_DATA,
  HOG_ERR_CMD_NOT_FOUND,

  HOG_ERR_PARSER,

  // errno is stored in err_detail
  HOG_ERRNO
};

const char *hog_err_to_str(enum hog_error self);

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

enum hog_error hog_err_print(FILE *f);

// return error code
enum hog_error hog_err(void);

#endif
