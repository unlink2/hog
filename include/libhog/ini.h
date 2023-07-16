#ifndef INI_H_
#define INI_H_

#include <stddef.h>

/**
 * Config file for more complex format settings
 * Reads a linear file line by line
 * and returns either a section or key/value struct
 */

// [section name]
struct hog_ini_section {
  const char *name;
  size_t name_len;
};

// key=value
struct hog_ini_kv {
  const char *key;
  size_t key_len;

  const char *value;
  size_t value_len;
};

enum hog_ini_type { HOG_INI_INVAL = 0, HOG_INI_SECTION, HOG_INI_KV };

struct hog_ini {
  enum hog_ini_type type;
  union {
    struct hog_ini_kv kv;
    struct hog_ini_section section;
  };
  // pointer to next char to parse after
  // returned data
  const char *next;
};

struct hog_ini hog_ini_init(void);

// get the next value from a stream of data
struct hog_ini hog_ini_next(const char *input);

#endif
