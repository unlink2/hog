#include "libhog/test/machine.h"
#include "libhog/config.h"
#include "libhog/error.h"
#include "libhog/test/test.h"
#include "libhog/machine.h"
#include "libhog/parser.h"
#include <cmocka.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define setup(usr_input, main)                                                 \
  struct hog_vm vm = hog_vm_init(100, tmpfile(), tmpfile(), tmpfile());        \
  fputs(usr_input, vm.stdin);                                                  \
  rewind(vm.stdin);                                                            \
  hog_parse_all(&vm);                                                          \
  hog_vm_main(&vm, (main));

#define assert_vm(out_expect)                                                  \
  char buf[1024];                                                              \
  memset(buf, 0, 1024);                                                        \
  assert_false(hog_err());                                                     \
  assert_true(vm.hlt);                                                         \
  rewind(vm.stdout);                                                           \
  fgets(buf, 1024, vm.stdout);                                                 \
  assert_string_equal((out_expect), buf);

#define teardown()                                                             \
  fclose(vm.stdin);                                                            \
  fclose(vm.stdout);                                                           \
  fclose(vm.fin);                                                              \
  hog_vm_free(&vm);                                                            \
  hog_err_set(HOG_OK)

void test_machine(void **state) {
  {
    setup("b%xp12 .e", NULL);
    assert_vm("c");
    teardown();
  }
  {
    setup("#this is a comment\n b%xp12 .e", NULL);
    assert_vm("c");
    teardown();
  }
  {
    setup("b%xp12 p13 p14 PP .e", NULL);
    assert_vm("c");
    teardown();
  }
  {
    setup("b%xp12 D ..e", NULL);
    assert_vm("cc");
    teardown();
  }
  {
    setup("NNN :print b .r :main b%xp12 D wpprint c c e", "main");
    assert_vm("cc");
    teardown();
  }
}
