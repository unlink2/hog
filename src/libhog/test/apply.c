#include "libhog/buffer.h"
#include "libhog/config.h"
#include "libhog/test/ini.h"
#include "libhog/error.h"
#include "libhog/test/test.h"
#include "libhog/apply.h"
#include <cmocka.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

void test_apply_int(void **state) {
  struct hog_config cfg = hog_config_init_builtins();
  struct hog_rc rc = hog_rc_init(&cfg);
  struct hog_buffer buf = hog_buffer_init();

  const uint8_t data[] = {12, 34};

  hog_apply_lookup(&rc, &buf, data, 2, "u8", 0);
  assert_false(hog_err());
  assert_string_equal("u8 test_name = 12", buf.data);

  hog_buffer_free(&buf);
  hog_config_free(&cfg);
}
