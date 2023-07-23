#include <libhog/macros.h>
#include <libhog/error.h>
#include <stdlib.h>
#include <string.h>

_Thread_local char ERR_MESSAGE[HOG_ERR_MSG_LEN];
_Thread_local char ERR_DETAILS[HOG_ERR_DETAIL_LEN];
_Thread_local enum hog_error HOG_ERR;

const char *hog_err_to_str(enum hog_error self) {
  switch (self) {
  case HOG_OK:
    return "OK";
  case HOG_ERRNO:
    return "ERRNO";
  case HOG_ERR_UNTERMINATED_TOKEN:
    return "Unterminated token";
  case HOG_ERR_INI_SECTION_INVAL:
    return "Ini inval section";
  case HOG_ERR_INI_BAD_KEY:
    return "Ini bad key";
  case HOG_ERR_CMD_INVAL:
    return "Invalid command";
  case HOG_ERR_TYPE_NOT_FOUND:
    return "Type not found";
  case HOG_ERR_OUT_OF_DATA:
    return "Out of data";
  case HOG_ERR_CMD_NOT_FOUND:
    return "Command not found";
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

void hog_err_reset(void) { HOG_ERR = HOG_OK; }

char *hog_err_msg_ptr(void) { return ERR_MESSAGE; }

size_t hog_err_msg_len(void) { return HOG_ERR_MSG_LEN; }

void *hog_err_details_ptr(void) { return ERR_DETAILS; }

size_t hog_err_details_len(void) { return HOG_ERR_DETAIL_LEN; }
