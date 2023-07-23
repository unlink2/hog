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
  hog_err_set(HOG_OK);                                                         \
  struct hog_config cfg = hog_config_init_builtins();                          \
  struct hog_rc rc = hog_rc_init(&cfg);                                        \
  struct hog_buffer buf = hog_buffer_init();

#define hog_teardown()                                                         \
  hog_buffer_free(&buf);                                                       \
  hog_config_free(&cfg);                                                       \
  hog_rc_free(&rc);

#define hog_expect(expect, type, data, len, new_offset)                        \
  {                                                                            \
    hog_buffer_clear(&buf);                                                    \
    size_t res = hog_apply_lookup(&rc, &buf, (data), (len), (type), 0);        \
    assert_int_equal((new_offset), res);                                       \
    assert_false(hog_err());                                                   \
    assert_string_equal((expect), (char *)buf.data);                           \
  }

void test_apply_int(void **state) {
  // decimal
  {
    hog_setup();
    hog_rc_name(&rc, "test_name");
    const uint8_t data[] = {-2, 34};
    hog_expect("u8 test_name = 254", "u8", data, 2, 1);
    hog_expect("i8 test_name = -2", "i8", data, 2, 1);
    hog_teardown();
  }
  {
    hog_setup();
    hog_rc_name(&rc, "test_name");
    const uint8_t data[] = {0x12, 0x34};
    hog_expect("u16 test_name = 13330", "u16", data, 2, 2);
    hog_expect("i16 test_name = 13330", "i16", data, 2, 2);

    hog_teardown();
  }
  {
    hog_setup();
    hog_rc_name(&rc, "test_name");
    const uint8_t data[] = {0x12, 0x34, 0x56, 0x78};
    hog_expect("u32 test_name = 2018915346", "u32", data, 4, 4);
    hog_expect("i32 test_name = 2018915346", "i32", data, 4, 4);

    hog_teardown();
  }
  {
    hog_setup();
    hog_rc_name(&rc, "test_name");
    const uint8_t data[] = {0x12, 0x34, 0x56, 0x78, 0x12, 0x34, 0x56, 0x78};
    hog_expect("u64 test_name = 8671175386481439762", "u64", data, 8, 8);
    hog_expect("i64 test_name = 8671175386481439762", "i64", data, 8, 8);

    hog_teardown();
  }
  {
    hog_setup();
    hog_rc_name(&rc, "test_name");
    cfg.arch_size = 4;
    const uint8_t data[] = {0x12, 0x34, 0x56, 0x78};
    hog_expect("usize test_name = 2018915346", "usize", data, 4, 4);
    hog_expect("isize test_name = 2018915346", "isize", data, 4, 4);

    hog_teardown();
  }
  {
    hog_setup();
    hog_rc_name(&rc, "test_name");
    cfg.arch_size = 4;
    const uint8_t data[] = {0x12, 0x34, 0x56, 0x78};
    hog_expect("void* test_name = 2018915346", "void*", data, 4, 4);

    hog_teardown();
  }

  // hex
  {
    hog_setup();
    hog_rc_name(&rc, "test_name");
    cfg.arch_size = 4;
    rc.int_fmt = HOG_FMT_INT_HEX;
    const uint8_t data[] = {0x12, 0x34, 0x56, 0x78};
    hog_expect("void* test_name = 0x78563412", "void*", data, 4, 4);

    // same with BE
    rc.endianess = HOG_ENDIAN_BIG;
    hog_expect("void* test_name = 0x12345678", "void*", data, 4, 4);

    hog_teardown();
  }

  // bin
  {
    hog_setup();
    hog_rc_name(&rc, "test_name");
    cfg.arch_size = 4;
    rc.int_fmt = HOG_FMT_INT_BIN;
    const uint8_t data[] = {0x12, 0x34, 0x56, 0x78};
    hog_expect("void* test_name = 0b1111000010101100011010000010010", "void*",
               data, 4, 4);

    hog_teardown();
  }

  // char
  {
    hog_setup();
    hog_rc_name(&rc, "test_name");
    cfg.arch_size = 4;
    rc.int_fmt = HOG_FMT_INT_CHAR;
    const uint8_t data[] = {0x21, 0x34, 0x56, 0x78};
    hog_expect("u8 test_name = '!'", "u8", data, 4, 1);

    hog_teardown();
  }

  // not enough data
  {
    hog_setup();
    hog_rc_name(&rc, "test_name");
    cfg.arch_size = 4;
    const uint8_t data[] = {0x12, 0x34, 0x56};
    hog_apply_lookup(&rc, &buf, data, 3, "void*", 0);
    assert_int_equal(HOG_ERR_OUT_OF_DATA, hog_err());

    hog_teardown();
  }
}

void test_apply_float(void **state) {
  {
    hog_setup();
    hog_rc_name(&rc, "test_name");

    const uint32_t d = 0x4880a008;
    const uint8_t *data = (uint8_t *)&d;
    hog_expect("f32 test_name = 263424.250000", "f32", data, 4, 4);

    hog_teardown();
  }
  {
    hog_setup();
    hog_rc_name(&rc, "test_name");

    const uint64_t d = 0x4110140104000180;
    const uint8_t *data = (uint8_t *)&d;
    hog_expect("f64 test_name = 263424.253906", "f64", data, 8, 8);

    hog_teardown();
  }
}

void test_apply_scope(void **state) {
  {
    hog_setup();
    hog_rc_name(&rc, "test_name");
    cfg.arch_size = 4;
    rc.scope_level = 2;
    const uint8_t data[] = {0x12, 0x34, 0x56, 0x78};
    hog_expect("    void* test_name = 2018915346", "void*", data, 4, 4);
    hog_teardown();
  }
}
