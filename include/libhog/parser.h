#ifndef PARSER_H_
#define PARSER_H_

#include "libhog/config.h"

// takes a textual representation of a struct
// and converts it to commands that are added to
// the command list
// returns the index of the newly created command list
size_t hog_parse(struct hog_config *cfg, const char *input);

#endif
