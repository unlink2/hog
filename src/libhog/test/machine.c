#include "libhog/test/machine.h"
#include "libhog/config.h"
#include "libhog/error.h"
#include "libhog/test/test.h"
#include "libhog/machine.h"
#include <cmocka.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

struct hog_vm setup(void) {
  struct hog_config cfg = hog_config_init();
  struct hog_vm self = hog_vm_init(&cfg);
  hog_config_free(&cfg);
  return self;
}

void teardown(struct hog_vm *self) { hog_vm_free(self); }

void test_machine(void **state) {
  {
    struct hog_vm v = setup();

    teardown(&v);
  }
}
