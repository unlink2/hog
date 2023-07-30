#include "libhog/parser.h"
#include "libhog/test/test.h"
#include "libhog/error.h"
#include "libhog/test/parser.h"
#include <cmocka.h>

#define hog_expect(input, expect_str)                                          \
  {                                                                            \
    struct hog_tok res = hog_parse_next((input));                              \
    char s[512];                                                               \
    hog_parse_tok_sprintf((char *)s, res);                                     \
    assert_string_equal((expect_str), s);                                      \
  }

void test_parser(void **state) {
  hog_expect("", "[END] ''");
  hog_expect("struct", "[STRUCT] 'struct'");
  hog_expect("   \t   struct", "[STRUCT] 'struct'");
  hog_expect("name123", "[IDENT] 'name123'");
  hog_expect("_name123", "[IDENT] '_name123'");
  hog_expect("1_name123", "[UNKNOWN] '1_name123'");
  hog_expect(";", "[SEMICOLON] ';'");
  hog_expect("{", "[BLK_OPEN] '{'");
  hog_expect("}", "[BLK_CLOSE] '}'");
  hog_expect("#", "[DIRECTIVE] '#'");

  hog_expect("_name123; name456", "[IDENT] '_name123'");
}
