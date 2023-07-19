#include "libhog/config.h"
#include "libhog/types.h"
#include <string.h>

struct hog_config hog_config_init(void) {
  struct hog_config c;
  memset(&c, 0, sizeof(c));

  return c;
}

void hog_config_free(struct hog_config *self) {
  hog_type_map_vec_free(&self->types_map);
}
