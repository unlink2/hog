#include "libhog/ini.h"
#include "libhog/error.h"
#include <ctype.h>
#include <string.h>

struct hog_ini hog_ini_init(void) {
  struct hog_ini self;
  memset(&self, 0, sizeof(self));

  return self;
}

struct hog_ini hog_ini_next(const char *input) {
  // first trim
  while (isspace(*input)) {
    input++;
  }

  const char *end = input;
  struct hog_ini self = hog_ini_init();

  while (*end != '\n' && *end != '\0') {
    end++;
  }

  if (*end == '\0') {
    self.next = NULL;
  } else {
    self.next = end + 1;
  }

  // decide if kv or section
  if (*input == '[') {
    self.type = HOG_INI_SECTION;
    self.section.name = ++input;
    while (input < end && *input != ']') {
      self.section.name_len++;
      input++;
    }
    if (*input != ']') {
      self.type = HOG_INI_INVAL;
      hog_err_set(HOG_ERR_INI_SECTION_INVAL);

      return self;
    }
  } else {
  }

  return self;
}
