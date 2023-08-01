#include "libhog/parser.h"
#include <ctype.h>

const char *hog_tok_next(const char *input, size_t *len) {
  while (*input != '\0' && isspace(*input)) {
    input++;
  }

  const char *start = input;

  *len = 0;
  if (*start == '\0') {
    return NULL;
  }

  while (*input != '\0' && !isspace(*input)) {
    input++;
    *len += 1;
  }

  return start;
}
