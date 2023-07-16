#include "libhog/test/test.h"
#include "libhog/test/vec.h"
#include "libhog/test/tok.h"
#include "libhog/test/buffer.h"
#include "libhog/test/ini.h"

int main(int arc, char **argv) {
  const struct CMUnitTest tests[] = {
      cmocka_unit_test(test_vec),
      cmocka_unit_test(test_buffer),
      cmocka_unit_test(test_tok),
      cmocka_unit_test(test_ini),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
