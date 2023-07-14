#include <stdio.h>
#include <stdlib.h>
#include "libhog/config.h"
#include "libhog/log.h"
#include "arg.h"

int main(int argc, char **argv) {
  struct hog_config cfg = hog_args_to_config(argc, argv);
  return 0;
}
