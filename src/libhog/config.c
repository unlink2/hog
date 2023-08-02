#include "libhog/config.h"
#include "libhog/error.h"
#include "libhog/log.h"
#include "libhog/macros.h"
#include "libhog/io.h"
#include <stdio.h>
#include <string.h>

struct hog_config hog_config_init(void) {
  struct hog_config self;
  memset(&self, 0, sizeof(self));

  self.stdin = fileno(stdin);
  self.stdout = fileno(stdout);
  self.fin = fileno(stdin);

  return self;
}

void hog_config_free(struct hog_config *self) {}
