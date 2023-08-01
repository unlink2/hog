#include "libhog/test/test.h"
#include "libhog/error.h"
#include "libhog/test/machine.h"

static int test_setup(void **state) {
  hog_err_set(HOG_OK);

  return 0;
}

int main(int arc, char **argv) {
  const struct CMUnitTest tests[] = {
      cmocka_unit_test_setup(test_machine, test_setup),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
