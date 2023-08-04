#ifndef COLOR_H_
#define COLOR_H_

#include <unistd.h>

#define HOG_ANSI_COLOR_RED "\x1b[31m"
#define HOG_ANSI_COLOR_GREEN "\x1b[32m"
#define HOG_ANSI_COLOR_YELLOW "\x1b[33m"
#define HOG_ANSI_COLOR_BLUE "\x1b[34m"
#define HOG_ANSI_COLOR_MAGENTA "\x1b[35m"
#define HOG_ANSI_COLOR_CYAN "\x1b[36m"
#define HOG_ANSI_COLOR_RESET "\x1b[0m"

#define hog_term_escape(stream, code)                                          \
  if (isatty(fileno((stream)))) {                                              \
    fputs((code), (stream));                                                   \
  }

#endif
