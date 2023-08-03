#include <libhog/macros.h>
#include <libhog/error.h>
#include <stdlib.h>
#include <string.h>

_Thread_local enum hog_error HOG_ERR;

void hog_err_set(enum hog_error err) { HOG_ERR = err; }

enum hog_error hog_err(void) {
  enum hog_error latest = HOG_ERR;
  return latest;
}
