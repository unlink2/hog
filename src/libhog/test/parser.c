#include "libhog/parser.h"
#include "libhog/test/test.h"
#include <cmocka.h>
#include <stdlib.h>
#include <string.h>

void test_tok(void **state) {
  FILE *f = tmpfile();
  fputs("test   tokens  \nNext", f);
  rewind(f);

  const char buffer[64];

  size_t n = hog_tok_next(f, (char *)buffer, 64);
  assert_string_equal("test", buffer);
  assert_int_equal(n, 4);

  n = hog_tok_next(f, (char *)buffer, 64);
  assert_string_equal("tokens", buffer);
  assert_int_equal(n, 6);

  n = hog_tok_next(f, (char *)buffer, 64);
  assert_string_equal("", buffer);
  assert_int_equal(0, n);

  n = hog_tok_next(f, (char *)buffer, 64);
  assert_string_equal("Next", buffer);
  assert_int_equal(n, 4);

  n = hog_tok_next(f, (char *)buffer, 64);
  assert_string_equal("", buffer);
  assert_int_equal(0, n);

  fclose(f);
}

void test_parser(void **state) {}
