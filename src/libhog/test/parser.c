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
  hog_vm_free(&vm);                                                            \
  hog_err_set(HOG_OK)

void test_parser(void **state) {
  {
    setup("\"\\\"Test String\\\"\"");

    hog_parse(&vm);
    assert_false(hog_err());
    assert_string_equal("\"Test String\"", (char *)vm.mem);

    teardown();
  }
  {
    setup("\"\\\"Test\\\"");

    hog_parse(&vm);
    assert_int_equal(HOG_ERR_PARSE_UNTERMINATED_STRING, hog_err());

    teardown();
  }
  {
    const int8_t expected = 123;

    setup("bp123");
    hog_parse_all(&vm);
    assert_false(hog_err());
    assert_int_equal(HOG_OP_T8, *vm.mem);
    assert_int_equal(HOG_OP_PUSH, *(vm.mem + 1));
    assert_memory_equal(&expected, vm.mem + 2, sizeof(expected));

    teardown();
  }
  {
    const int16_t expected = 12345;

    setup("sp12345");
    hog_parse_all(&vm);
    assert_false(hog_err());
    assert_int_equal(HOG_OP_T16, *vm.mem);
    assert_int_equal(HOG_OP_PUSH, *(vm.mem + 1));
    assert_memory_equal(&expected, vm.mem + 2, sizeof(expected));

    teardown();
  }
  {
    const int32_t expected = 1234567;

    setup("ip1234567");
    hog_parse_all(&vm);
    assert_false(hog_err());
    assert_int_equal(HOG_OP_T32, *vm.mem);
    assert_int_equal(HOG_OP_PUSH, *(vm.mem + 1));
    assert_memory_equal(&expected, vm.mem + 2, sizeof(expected));

    teardown();
  }
  {
    const int64_t expected = 1234567;

    setup("lp1234567");
    hog_parse_all(&vm);
    assert_false(hog_err());
    assert_int_equal(HOG_OP_T64, *vm.mem);
    assert_int_equal(HOG_OP_PUSH, *(vm.mem + 1));
    assert_memory_equal(&expected, vm.mem + 2, sizeof(expected));

    teardown();
  }
  {
    const double expected = 3.1415F;

    setup("dp3.1415");
    hog_parse_all(&vm);
    assert_false(hog_err());
    assert_int_equal(HOG_OP_TD, *vm.mem);
    assert_int_equal(HOG_OP_PUSH, *(vm.mem + 1));
    assert_memory_equal(&expected, vm.mem + 2, sizeof(expected));

    teardown();
  }
  {
    const float expected = 3.1415F;

    setup("fp3.1415");
    hog_parse_all(&vm);
    assert_false(hog_err());
    assert_int_equal(HOG_OP_TF, *vm.mem);
    assert_int_equal(HOG_OP_PUSH, *(vm.mem + 1));
    assert_memory_equal(&expected, vm.mem + 2, sizeof(expected));

    teardown();
  }
  {
    const char expected = 'c';

    setup("bp'c'");
    hog_parse_all(&vm);
    assert_false(hog_err());
    assert_int_equal(HOG_OP_T8, *vm.mem);
    assert_int_equal(HOG_OP_PUSH, *(vm.mem + 1));
    assert_memory_equal(&expected, vm.mem + 2, sizeof(expected));

    teardown();
  }
  {
    const char expected = '\\';

    setup("bp'\\\\'");
    hog_parse_all(&vm);
    assert_false(hog_err());
    assert_int_equal(HOG_OP_PUSH, *(vm.mem + 1));
    assert_memory_equal(&expected, vm.mem + 2, sizeof(expected));

    teardown();
  }
  {
    setup("bp'c");
    hog_parse_all(&vm);
    assert_int_equal(HOG_ERR_PARSE_UNTERMINATED_CHAR, hog_err());

    teardown();
  }
}
