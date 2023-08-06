#include "arg.h"
#include "libhog/config.h"
#include "libhog/log.h"
#include <unistd.h>
#include <getopt.h>

// output params
char progname[] = "hog";
char short_desc[] = "";

// version info
int version_major = 0;
int version_minor = 0;
int version_patch = 1;

void hog_args_syntax(void) {}

void hog_args_glossary(void) {}

void hog_args_print_help(void) {
  hog_info(stderr, "Usage: %s", progname);
  hog_args_syntax();
  hog_info(stderr, "%s\n\n", short_desc);
  hog_args_glossary();
  exit(1); // NOLINT
}

void hog_args_print_version(void) {
  hog_info("%s version %d.%d.%d\n", progname, version_major, version_minor,
           version_patch);
}

void hog_args_print_err(void) {
  hog_error(stderr, "Try '%s --help' for more information.\n", progname);
  exit(-1); // NOLINT
}

struct hog_config hog_args_to_config(int argc, char **argv) {
  int option_index = 0;
  int opt = 0;
  static struct option long_options[] = {
      {"help", no_argument, 0, 'h'},
      {"version", no_argument, 0, 'V'},
      {"verbose", no_argument, 0, 'v'},
      {"script", required_argument, 0, 's'},
      {"output", required_argument, 0, 'o'},
      {"eval", required_argument, 0, 'e'},
      {"help-expr", no_argument, 0, 0},
      {"no-interactive", no_argument, 0, 't'},
      {"input", required_argument, 0, 'i'},
      {0, 0, 0, 0}};

  struct hog_config cfg = hog_config_init();

  while ((opt = getopt_long(argc, argv, "hvVsoeti", long_options, // NOLINT
                            &option_index)) != -1) {
    switch (opt) {
    case 'h':
    case '?':
      hog_args_print_help();
      break;
    case 'V':
      hog_args_print_version();
      break;
    }
  }

  if (optind >= argc) {
    hog_error(stderr, "Missing positional argument 'input'.\n");
    hog_args_print_err();
  } else if (optind + 1 == argc) {
    cfg.fin_path = argv[optind++];
  } else {
    hog_error(stderr, "Supplied too may postional arguments!\n");
    hog_args_print_err();
  }

  return cfg;
}
