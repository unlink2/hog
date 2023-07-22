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
    hog_expect("i16 test_name = 13330", "i16", data, 2);

    hog_teardown();
  }
  {
    hog_setup();
    hog_rc_name(&rc, "test_name");
    const uint8_t data[] = {0x12, 0x34, 0x56, 0x78};
    hog_expect("u32 test_name = 2018915346", "u32", data, 4);
    hog_expect("i32 test_name = 2018915346", "i32", data, 4);

    hog_teardown();
  }
  {
    hog_setup();
    hog_rc_name(&rc, "test_name");
    const uint8_t data[] = {0x12, 0x34, 0x56, 0x78, 0x12, 0x34, 0x56, 0x78};
    hog_expect("u64 test_name = 8671175386481439762", "u64", data, 8);
    hog_expect("i64 test_name = 8671175386481439762", "i64", data, 8);

    hog_teardown();
  }
  {
    hog_setup();
    hog_rc_name(&rc, "test_name");
    cfg.arch_size = 4;
    const uint8_t data[] = {0x12, 0x34, 0x56, 0x78};
    hog_expect("usize test_name = 2018915346", "usize", data, 4);
    hog_expect("isize test_name = 2018915346", "isize", data, 4);

    hog_teardown();
  }
  {
    hog_setup();
    hog_rc_name(&rc, "test_name");
    cfg.arch_size = 4;
    const uint8_t data[] = {0x12, 0x34, 0x56, 0x78};
    hog_expect("void* test_name = 2018915346", "void*", data, 4);

    hog_teardown();
  }
}
