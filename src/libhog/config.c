#include "libhog/config.h"
#include <string.h>

struct hog_config hog_config_init(void) {
  struct hog_config c;
  memset(&c, 0, sizeof(c));

  return c;
}
