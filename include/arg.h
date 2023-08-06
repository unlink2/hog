#ifndef ARG_H_
#define ARG_H_

#include <stdlib.h>

#include "libhog/config.h"
#include "libhog/log.h"
#include "libhog/macros.h"

struct hog_config hog_args_to_config(int argc, char **argv);

#endif
