#include "libhog/command.h"
#include <string.h>

struct hog_cmd hog_cmd_init(void) {
  struct hog_cmd self;
  memset(&self, 0, sizeof(self));

  return self;
}

struct hog_cmd hog_cmd_move_init(int move_bytes) {
  struct hog_cmd self = hog_cmd_init();
  self.move_bytes = move_bytes;
  self.type = HOG_CMD_MOVE_BYTES;

  return self;
}
