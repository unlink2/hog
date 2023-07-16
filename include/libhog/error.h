#ifndef ERROR_H_
#define ERROR_H_

#include "libhog/macros.h"
#include <stddef.h>
#include <errno.h>
#include <stdio.h>

/**
 * Error is a error type struct that provides
 * an errno-style interface.
 * It provides a generic message location as well as a details buffer
 * where additional error information can  be stored.
 * Both of those locations are thread local and therefore
 * are OK to overwirte at any point.
 * It is up to the caller to handle and read the buffers if
 * an error is returned. Do not rely on the buffer not being overwirtten after
 * making a new call! The error info struct should be included in structs.
 */

#define HOG_ERR_MSG_LEN 1024
#define HOG_ERR_DETAIL_LEN 1024

// Possible error types.
// Some may or may not require special error info
enum hog_error {
  HOG_OK = 0,
  HOG_ERR_UNTERMINATED_TOKEN,
  HOG_ERR_INI_SECTION_INVAL,
  HOG_ERR_INI_BAD_KEY,
  HOG_ERR_CMD_INVAL,
  // errno is stored in err_detail
  HOG_ERRNO
};

const char *hog_err_to_str(enum hog_error self);

#define hog_err_msg(...) sprintf(hog_err_msg_ptr(), __VA_ARGS__);

#define hog_errno()                                                            \
  {                                                                            \
    hog_err_set(HOG_ERRNO);                                                    \
    hog_err_detail(&errno, sizeof(errno));                                     \
  }

#define hog_err_detail(data, len) memcpy(hog_err_details_ptr(), data, len);

// sets err to a value
// to indicate a failure in a call
void hog_err_set(enum hog_error err);

void hog_err_print(FILE *f);

// return error code and resets value
// to OK
enum hog_error hog_err(void);
void hog_err_rest(void);

// the error message is in practice a thread local
// string buffer where errors may write specific information
// about the kind of failure. It should only ever be obtained
// via this function.
// This of course means that subsequent errors may overwrite
// the previous error message! Ensure to handle errors in a timely manner
// to avoid losing error information
char *hog_err_msg_ptr(void);
size_t hog_err_msg_len(void);

// Error details can be used to store metadata about the error
// The data type depends on the error kind and should be documented above
// if available
void *hog_err_details_ptr(void);
size_t hog_err_details_len(void);

// bail on error macro
#define hog_ok_or(err, ret)                                                    \
  {                                                                            \
    if ((err)) {                                                               \
      return (ret);                                                            \
    }                                                                          \
  }

// optional struct
#define hog_some_or(optional, ret)                                             \
  {                                                                            \
    if ((optional)) {                                                          \
      return (ret);                                                            \
    }                                                                          \
  }

#endif
