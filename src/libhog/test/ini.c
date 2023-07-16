#include "libhog/test/ini.h"
#include "libhog/error.h"
#include "libhog/test/test.h"
#include "libhog/ini.h"
#include <cmocka.h>
#include <stdlib.h>
#include <string.h>

#define assert_string_equal_len(expected, s, len)                              \
  {                                                                            \
    char *l = strndup(s, len);                                                 \
    assert_string_equal(expected, l);                                          \
    free(l);                                                                   \
  }

void test_ini(void **state) {
  // single section
  {
    const char *section_test = "[test]";
    struct hog_ini section = hog_ini_next(section_test);

    assert_false(hog_err());
    assert_string_equal_len("test", section.section.name,
                            section.section.name_len);
    assert_int_equal(HOG_INI_SECTION, section.type);
    assert_null(section.next);
  }
  // single value
  {
    const char *section_test = "key=value test";
    struct hog_ini val = hog_ini_next(section_test);

    assert_false(hog_err());
    assert_string_equal_len("key", val.kv.key, val.kv.key_len);
    assert_string_equal_len("value test", val.kv.value, val.kv.value_len);
    assert_int_equal(HOG_INI_KV, val.type);
    assert_null(val.next);
  }

  // empty value
  {
    const char *section_test = "key";
    struct hog_ini val = hog_ini_next(section_test);

    assert_false(hog_err());
    assert_string_equal_len("key", val.kv.key, val.kv.key_len);
    assert_null(val.kv.value);
    assert_int_equal(HOG_INI_KV, val.type);
    assert_null(val.next);
  }

  // "complex" file
  {
    const char *input = "  test=  123  \n  [section]  \nkey=value\n   ";

    struct hog_ini val = hog_ini_next(input);
    assert_false(hog_err());
    assert_string_equal_len("test", val.kv.key, val.kv.key_len);
    assert_string_equal_len("123", val.kv.value, val.kv.value_len);
    assert_int_equal(HOG_INI_KV, val.type);
    assert_non_null(val.next);

    val = hog_ini_next(val.next);
    assert_false(hog_err());
    assert_string_equal_len("section", val.section.name, val.section.name_len);
    assert_int_equal(HOG_INI_SECTION, val.type);
    assert_non_null(val.next);

    val = hog_ini_next(val.next);
    assert_false(hog_err());
    assert_string_equal_len("key", val.kv.key, val.kv.key_len);
    assert_string_equal_len("value", val.kv.value, val.kv.value_len);
    assert_int_equal(HOG_INI_KV, val.type);
    assert_non_null(val.next);

    val = hog_ini_next(val.next);
    assert_false(hog_err());
    assert_int_equal(HOG_INI_EMPTY, val.type);
    assert_null(val.next);
  }

  // invalid section
  {
    const char *section_test = "[test";
    hog_ini_next(section_test);

    assert_int_equal(HOG_ERR_INI_SECTION_INVAL, hog_err());
  }
}
