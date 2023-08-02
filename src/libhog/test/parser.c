#include "libhog/parser.h"
#include "libhog/error.h"
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

#define setup(usr_input)                                                       \
  struct hog_vm vm = hog_vm_init(100, tmpfile(), tmpfile(), tmpfile());        \
  fputs(usr_input, vm.stdin);                                                  \
  rewind(vm.stdin);

#define teardown()                                                             \
  fclose(vm.stdin);                                                            \
  fclose(vm.stdout);                                                           \
  fclose(vm.fin);                                                              \
  hog_vm_free(&vm);

void test_parser(void **state) {
  {
    setup("\"\\\"Test String\\\"\"");

    hog_parse(&vm);
    assert_false(hog_err());
    assert_string_equal("\"Test String\"", vm.mem);

    teardown();
  }
  {
    setup("\"\\\"Test\\\"");

    hog_parse(&vm);
    assert_int_equal(HOG_ERR_PARSE_UNTERMINATED_STRING, hog_err());

    teardown();
  }
}
