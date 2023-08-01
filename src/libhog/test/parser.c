#include "libhog/parser.h"
#include "libhog/test/test.h"
#include <cmocka.h>
#include <stdlib.h>
#include <string.h>

const char *test_tokens = "test   tokens";
size_t test_tokens_idx = 0;

size_t test_tok_read(int fd, void *buffer, size_t len) {
  if (test_tokens[test_tokens_idx] == '\0') {
    return -1;
  }

  memcpy(buffer, test_tokens + test_tokens_idx, len);
  test_tokens_idx += len;
  return len;
}

void test_tok(void **state) {
  test_tokens_idx = 0;
  const char buffer[64];

  size_t n = hog_tok_next(test_tok_read, 0, (char *)buffer, 64);
  assert_string_equal("test", buffer);
  assert_int_equal(n, 4);

  n = hog_tok_next(test_tok_read, 0, (char *)buffer, 64);
  assert_string_equal("tokens", buffer);
  assert_int_equal(n, 6);

  n = hog_tok_next(test_tok_read, 0, (char *)buffer, 64);
  assert_string_equal("", buffer);
  assert_int_equal(0, n);
}

void test_parser(void **state) {}
