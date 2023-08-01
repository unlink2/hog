#include "libhog/parser.h"
#include "libhog/test/test.h"
#include <cmocka.h>
#include <stdlib.h>

void test_parser(void **state) {

  size_t n = 0;
  assert_memory_equal("test", hog_tok_next("test tokens", &n), 4);
  assert_int_equal(n, 4);

  assert_memory_equal("tokens", hog_tok_next("   tokens", &n), 6);
  assert_int_equal(n, 6);

  assert_null(hog_tok_next("", &n));
}
