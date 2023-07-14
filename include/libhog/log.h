#ifndef LOG_H_
#define LOG_H_

#include "assert.h"
#include <stdio.h>
#include <stdlib.h>

enum hog_loglevel {
  HOG_LOG_LEVEL_OFF,
  HOG_LOG_LEVEL_ERROR,
  HOG_LOG_LEVEL_WARN,
  HOG_LOG_LEVEL_INFO,
  HOG_LOG_LEVEL_DBG,
};

void hog_log_init(enum hog_loglevel level);

extern enum hog_loglevel HOG_LOG_LEVEL;
extern FILE *HOG_LOG_TARGET;

#define hog_dbg_assert(a) assert((a))
#define hog_log_fprintf(f, level, ...)                                         \
  if ((level) <= HOG_LOG_LEVEL) {                                              \
    /* fprintf((f), "[%d] ", level); */                                        \
    fprintf((f), __VA_ARGS__);                                                 \
  }

#define hog_dbg(...) hog_log_fprintf(stderr, HOG_LOG_LEVEL_DBG, __VA_ARGS__)
#define hog_warn(...) hog_log_fprintf(stderr, HOG_LOG_LEVEL_WARN, __VA_ARGS__)
#define hog_info(...) hog_log_fprintf(stderr, HOG_LOG_LEVEL_INFO, __VA_ARGS__)
#define hog_error(...) hog_log_fprintf(stderr, HOG_LOG_LEVEL_ERROR, __VA_ARGS__)

#define hog_panic(...)                                                         \
  {                                                                            \
    fprintf(stderr, __VA_ARGS__);                                              \
    exit(-1);                                                                  \
  }
#define hog_todo(...)                                                          \
  { hog_panic(__VA_ARGS__); }

#endif
