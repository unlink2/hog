#include <stdio.h>
#include <stdlib.h>
#include "libhog/config.h"
#include "libhog/log.h"
#include "arg.h"
#include "libhog/machine.h"

int main(int argc, char **argv) {
  struct hog_config cfg = hog_args_to_config(argc, argv);

  hog_vm_close_files(cfg.vm);
  hog_vm_free(cfg.vm);
  return 0;
}
