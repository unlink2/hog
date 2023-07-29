#include "libhog/parser.h"
#include <ctype.h>
#include <string.h>

bool hog_parse_is_term(char c) {
  return c == '\0' || isspace(c) || c == ';' || c == '{' || c == '}' ||
         c == '#';
}

struct hog_tok hog_parse_next(const char *input) {
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

  size_t len = input - start;

  // find type
  if (len == 1) {

    switch (*start) {
    case ';':
      res.type = HOG_TOK_SEMICOLON;
      break;
    case '{':
      res.type = HOG_TOK_BLK_OPEN;
      break;
    case '}':
      res.type = HOG_TOK_BLK_CLOSE;
      break;
    case '#':
      res.type = HOG_TOK_DIRECTIVE;
      break;
    }
  } else {
    if (strncmp("struct", start, len) == 0) {
      res.type = HOG_TOK_STRUCT;
    } else if (*start == '_' || isalpha(*start)) {
      bool ok = true;
      for (size_t i = 1; i < len && ok; i++) {
        ok = start[i] == '_' || isalnum(start[i]);
      }

      if (ok) {
        res.type = HOG_TOK_IDENT;
      }
    }
  }

  res.raw = start;
  res.raw_len = len;

  return res;
}

size_t hog_parse_struct(struct hog_config *cfg, const char *input) { return 0; }

size_t hog_parse_member(struct hog_config *cfg, const char *input) { return 0; }
