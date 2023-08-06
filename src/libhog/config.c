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
  self.log_level = HOG_LOG_LEVEL_INFO;
  return self;
}

void hog_config_build(struct hog_config *self) {
  hog_log_init(self->log_level);
}
