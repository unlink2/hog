#include "libhog/parser.h"
#include "libhog/error.h"
#include "libhog/log.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>

bool hog_parse_is_term(char c) {
  return c == '\0' || isspace(c) || c == ';' || c == '{' || c == '}' ||
         c == '#';
}

const char *hog_parse_tok_to_str(const enum hog_tok_type type) {
  const char *tok_name = NULL;
  switch (type) {
  case HOG_TOK_UNKNOWN:
    tok_name = "UNKNOWN";
    break;
  case HOG_TOK_END:
    tok_name = "END";
    break;
  case HOG_TOK_SEMICOLON:
    tok_name = "SEMICOLON";
    break;
  case HOG_TOK_STRUCT:
    tok_name = "STRUCT";
    break;
  case HOG_TOK_IDENT:
    tok_name = "IDENT";
    break;
  case HOG_TOK_DIRECTIVE:
    tok_name = "DIRECTIVE";
    break;
  case HOG_TOK_BLK_OPEN:
    tok_name = "BLK_OPEN";
    break;
  case HOG_TOK_BLK_CLOSE:
    tok_name = "BLK_CLOSE";
    break;
  }

  return tok_name;
}

size_t hog_parse_tok_sprintf(char *s, const struct hog_tok tok) {
  size_t idx = 0;

  const char *tok_name = hog_parse_tok_to_str(tok.type);

  idx = sprintf(s, "[%s] '%.*s'", tok_name, (int)tok.raw_len, tok.raw);

  return idx;
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

  // single char input
  if (len == 0) {
    if (*start == '\0') {
      res.type = HOG_TOK_END;
    } else {
      len = 1;
    }
  }

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
  } else if (len > 0) {
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

struct hog_tok hog_parse_expect(const char *input, enum hog_tok_type expected) {
  const struct hog_tok tok = hog_parse_next(input);
  if (tok.type != expected) {
    hog_err_fset(HOG_ERR_PARSER, "Expected type '%s' but got '%s:%.*s'",
                 hog_parse_tok_to_str(expected), hog_parse_tok_to_str(tok.type),
                 (int)tok.raw_len, tok.raw);
  }

  return tok;
}

size_t hog_parse_struct(struct hog_config *cfg, const char *input) { return 0; }

size_t hog_parse_member(struct hog_config *cfg, const char *input) { return 0; }
