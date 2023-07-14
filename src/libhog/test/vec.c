#include "libhog/test/test.h"
#include <cmocka.h>
#include "libhog/test/vec.h"
#include "libhog/vec.h"
#include <stdint.h>
#include <stdio.h>

void test_vec(void **state) {
  struct hog_vec v = hog_vec_init(sizeof(intptr_t));

  assert_null(hog_vec_pop(&v));
  assert_int_equal(0, v.len);

  for (int i = 0; i < hog_VEC_DEFAULT_MAX_LEN * 4; i++) {
    int d = 0 - i;
    hog_vec_push(&v, &d);
    assert_int_equal(i + 1, v.len);
  }

  for (int i = 0; i < hog_VEC_DEFAULT_MAX_LEN * 4; i++) {
    int *r = hog_vec_get(&v, i);
    assert_int_equal(0 - i, *r);
  }

  for (int i = 0; i < hog_VEC_DEFAULT_MAX_LEN * 4; i++) {
    int *p = hog_vec_pop(&v);
    assert_non_null(p);
    assert_int_equal(hog_VEC_DEFAULT_MAX_LEN * 4 - i - 1, v.len);
  }

  hog_vec_free(&v);
}
