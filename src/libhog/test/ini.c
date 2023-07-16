#include "libhog/test/ini.h"
#include "libhog/error.h"
#include "libhog/test/test.h"
#include "libhog/ini.h"
#include <cmocka.h>
#include <stdlib.h>

void test_ini(void **state) {
  const char *section_test = "[test]";
  struct hog_ini section = hog_ini_next(section_test);

  assert_false(hog_err());
}
