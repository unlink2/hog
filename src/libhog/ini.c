#include "libhog/ini.h"
#include "libhog/error.h"
#include <ctype.h>
#include <string.h>

struct hog_ini hog_ini_init(void) {
  struct hog_ini self;
  memset(&self, 0, sizeof(self));

  return self;
}

const char *hog_ini_trim(const char *input, size_t *len) {
  const char *begin = input;
  while (isspace(*input) && *input != '\0') {
    input++;
  }
  const char *start = input;

  input = begin + *len;
  while (isspace(*input) && *input != '\0') {
    input--;
  }
  const char *end = input + 1;

  *len = end - start;

  return start;
}

struct hog_ini hog_ini_next(const char *input) {
  // first trim
  while (isspace(*input) && *input != '\0') {
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

    self.section.name = hog_ini_trim(self.section.name, &self.section.name_len);
    self.section.name_len--;
  } else if (*input == '\0') {
    self.type = HOG_INI_EMPTY;
    self.next = NULL;
  } else {
    self.type = HOG_INI_KV;
    self.kv.key = input;

    while (input < end && *input != '=') {
      self.kv.key_len++;
      input++;
    }

    self.kv.key = hog_ini_trim(self.kv.key, &self.kv.key_len);
    if (self.kv.key[self.kv.key_len - 1] == '=') {
      self.kv.key_len--;
    }

    if (*input != '=') {
      self.kv.value = NULL;
    } else {
      self.kv.value = ++input;
      self.kv.value_len = end - input;

      self.kv.value = hog_ini_trim(self.kv.value, &self.kv.value_len);
    }
  }

  return self;
}
