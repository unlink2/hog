#include "libhog/log.h"

#include <stdint.h>

enum hog_loglevel HOG_LOG_LEVEL;
FILE *HOG_LOG_TARGET;

void hog_log_init(enum hog_loglevel level) {
  HOG_LOG_LEVEL = level;
  HOG_LOG_TARGET = stderr;
}
