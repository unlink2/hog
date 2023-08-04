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
    setup("s %x p0x8FEE .e", NULL);
    assert_vm("8fee");
    teardown();
  }
  {
    setup("i %x p0xA8FEE .e", NULL);
    assert_vm("a8fee");
    teardown();
  }
  {
    setup("d %x p0xFEDCBA8FEE .e", NULL);
    assert_vm("fedcba8fee");
    teardown();
  }
  {
    setup("f %f p3.1415 .e", NULL);
    assert_vm("3.141500");
    teardown();
  }
  {
    setup("d %f p3.1415 .e", NULL);
    assert_vm("3.141500");
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
    setup("NNN :print b.r:main b%xp12 D wpprint c  wpprint c  e", "main");
    assert_vm("cc");
    teardown();
  }
  {
    setup(":case1 b%d.e :main p12 wpcase1 j e", "main");
    assert_vm("12");
    teardown();
  }
  {
    // jump if 1
    setup(":case1 b%d.e :main p12 wpcase1 bp1 J e", "main");
    assert_vm("12");
    teardown();
  }
  {
    // jump if not
    setup(":case1 b%d.e :main p12 wpcase1 bp0 J bp10 . e", "main");
    assert_vm("10");
    teardown();
  }
  {
    // runtime word lookup
    setup(":c2_str \"case2\" :main wpc2_str L c b%dp11 . e :case2 b%dp12 . r",
          "main");
    assert_vm("1211");
    teardown();
  }
}
