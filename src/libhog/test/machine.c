#include "libhog/test/machine.h"
#include "libhog/config.h"
#include "libhog/error.h"
#include "libhog/test/test.h"
#include "libhog/machine.h"
#include <cmocka.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define BUF_LEN 1024

int8_t rd_buf[BUF_LEN];
size_t rd_idx;

int8_t wr_buf[BUF_LEN];
size_t wr_idx;

size_t test_write(int fd, const void *d, size_t n) {
  memcpy(wr_buf + wr_idx, d, n);
  wr_idx += n;
  return n;
}

size_t test_read(int fd, void *d, size_t n) {
  memcpy(d, rd_buf + rd_idx, n);
  rd_idx += n;
  return n;
}

struct hog_vm setup(void) {
  struct hog_config cfg = hog_config_init();

  cfg.fin = 3;
  cfg.stdout = 0;
  cfg.stdin = 1;
  cfg.read = test_read;
  cfg.write = test_write;

  struct hog_vm self = hog_vm_init(&cfg);

  memset(rd_buf, 0, BUF_LEN);
  memset(wr_buf, 0, BUF_LEN);
  rd_idx = 0;
  wr_idx = 0;

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
