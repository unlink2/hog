#ifndef INI_H_
#define INI_H_

/**
 * Config file for more complex format settings
 */

struct hog_ini_section {
  const char *name;
};

struct hog_ini_kv {
  const char *key;
  const char *value;
};

#endif
