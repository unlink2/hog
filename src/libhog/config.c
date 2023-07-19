#include "libhog/config.h"
#include "libhog/command.h"
#include "libhog/types.h"
#include "libhog/vec.h"
#include <string.h>

struct hog_config hog_config_init(void) {
  struct hog_config self;
  memset(&self, 0, sizeof(self));

  self.cmds = hog_vec_init(sizeof(struct hog_cmd));
  self.cmds_map = hog_vec_init(sizeof(struct hog_cmd_map));

  self.types = hog_vec_init(sizeof(struct hog_type));
  self.types_map = hog_vec_init(sizeof(struct hog_type_map));

  return self;
}

void hog_config_free(struct hog_config *self) {
  hog_type_vec_free(&self->types);
  hog_type_map_vec_free(&self->types_map);

  hog_cmd_map_vec_free(&self->cmds);
  hog_cmd_map_vec_free(&self->cmds_map);
}
