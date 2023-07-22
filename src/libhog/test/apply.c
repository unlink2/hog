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

#define hog_setup()                                                            \
  struct hog_config cfg = hog_config_init_builtins();                          \
  struct hog_rc rc = hog_rc_init(&cfg);                                        \
  struct hog_buffer buf = hog_buffer_init();

#define hog_teardown()                                                         \
  hog_buffer_free(&buf);                                                       \
  hog_config_free(&cfg);                                                       \
  hog_rc_free(&rc);

#define hog_expect(expect, type, data, len)                                    \
  {                                                                            \
    hog_buffer_clear(&buf);                                                    \
    hog_apply_lookup(&rc, &buf, (data), (len), (type), 0);                     \
    assert_false(hog_err());                                                   \
    assert_string_equal((expect), (char *)buf.data);                           \
  }

void test_apply_int(void **state) {
  {
    hog_setup();
    hog_rc_name(&rc, "test_name");
    const uint8_t data[] = {-2, 34};
    hog_expect("u8 test_name = 254", "u8", data, 2);
    hog_expect("i8 test_name = -2", "i8", data, 2);
    hog_teardown();
  }
  {
    hog_setup();
    hog_rc_name(&rc, "test_name");
    const uint8_t data[] = {0x12, 0x34};
    hog_expect("u16 test_name = 13330", "u16", data, 2);

    hog_teardown();
  }
}
