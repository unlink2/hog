#include "libhog/parser.h"
#include <ctype.h>

bool hog_parse_is_term(char c) { return c == '\0' || isspace(c) || c == ';'; }

struct hog_tok hog_parse_next(const char *input, size_t *len) {
  struct hog_tok res;
  res.type = HOG_TOK_UNKNOWN;

  // trim before parsing
  while (isspace(*input) && *input != '\0') {
    input++;
  }

  const char *start = input;

  while (!hog_parse_is_term(*input)) {
    input++;
  }

  res.raw = start;
  res.raw_len = input - start;

  return res;
}

size_t hog_parse_struct(struct hog_config *cfg, const char *input) { return 0; }

size_t hog_parse_member(struct hog_config *cfg, const char *input) { return 0; }
