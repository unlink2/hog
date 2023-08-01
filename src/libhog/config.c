#include "libhog/config.h"
#include "libhog/error.h"
#include "libhog/log.h"
#include "libhog/macros.h"
#include <stdio.h>
#include <string.h>

struct hog_config hog_config_init(void) {
  struct hog_config self;
  memset(&self, 0, sizeof(self));

  self.indent_char = ' ';
  self.indent_cnt = 2;
  self.scope_open = '{';
  self.scope_close = '}';
  self.array_open = '[';
  self.array_close = ']';
  self.array_sep = ',';
  self.new_line = '\n';

  return self;
}

void hog_config_free(struct hog_config *self) {}
