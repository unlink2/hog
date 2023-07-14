#include "libhog/error.h"
#include "libhog/test/test.h"
#include <cmocka.h>
#include <stdlib.h>
#include "libhog/test/tok.h"
#include "libhog/tok.h"

void test_tok(void **state) {
  const char *input =
      "  tok  test 123 'test token \\' with escaped chars' after";
  char buf[64];

  input = hog_tok_str(buf, input, 64);
  assert_false(hog_err());
  assert_string_equal("tok", buf);

  input = hog_tok_str(buf, input, 64);
  assert_false(hog_err());
  assert_string_equal("test", buf);

  input = hog_tok_str(buf, input, 64);
  assert_false(hog_err());
  assert_string_equal("123", buf);

  input = hog_tok_str(buf, input, 64);
  assert_false(hog_err());
  assert_string_equal("test token ' with escaped chars", buf);

  input = hog_tok_str(buf, input, 64);
  assert_false(hog_err());
  assert_string_equal("after", buf);

  input = hog_tok_str(buf, input, 64);
  assert_false(hog_err());
  assert_string_equal("", buf);

  // unterminated input
  const char *unterm = "'Unterminated";
  input = hog_tok_str(buf, unterm, 64);
  assert_int_equal(HOG_ERR_UNTERMINATED_TOKEN, hog_err());
}
