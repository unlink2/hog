#include "libhog/test/test.h"
#include "libhog/error.h"
#include "libhog/test/vec.h"
#include "libhog/test/tok.h"
#include "libhog/test/buffer.h"
#include "libhog/test/ini.h"
#include "libhog/test/apply.h"

static int test_setup(void **state) {
  hog_err_set(HOG_OK);

  return 0;
}

int main(int arc, char **argv) {
  const struct CMUnitTest tests[] = {
      cmocka_unit_test(test_vec),
      cmocka_unit_test(test_buffer),
      cmocka_unit_test(test_tok),
      cmocka_unit_test_setup(test_ini, test_setup),
      cmocka_unit_test_setup(test_apply_int, test_setup),
      cmocka_unit_test_setup(test_apply_float, test_setup),
      cmocka_unit_test_setup(test_apply_scope, test_setup),
      cmocka_unit_test_setup(test_apply_struct, test_setup),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
