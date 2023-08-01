#include <libhog/macros.h>
#include <libhog/error.h>
#include <stdlib.h>
#include <string.h>

_Thread_local enum hog_error HOG_ERR;

const char *hog_err_to_str(enum hog_error self) {
  switch (self) {
  case HOG_OK:
    return "OK";
  case HOG_ERRNO:
    return "ERRNO";
  }

  return "Unknown error";
}

enum hog_error hog_err_print(FILE *f) {
  enum hog_error err = hog_err();
  switch (err) {
  case HOG_OK:
    break;
  case HOG_ERRNO:
    fprintf(f, "%s\n", strerror(errno)); // NOLINT
    break;
  default:
    fprintf(f, "%s\n", hog_err_to_str(err));
  }

  hog_err_set(HOG_OK);
  return err;
}

void hog_err_set(enum hog_error err) { HOG_ERR = err; }

enum hog_error hog_err(void) {
  enum hog_error latest = HOG_ERR;
  return latest;
}
