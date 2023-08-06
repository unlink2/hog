#include "arg.h"
#include "libhog/config.h"
#include "libhog/log.h"
#include <string.h>
#include <unistd.h>
#include <getopt.h>

// output params
char progname[] = "hog";
char short_desc[] = "Binary manipulation calculator";

// version info
int version_major = 0;
int version_minor = 0;
int version_patch = 1;

const char *short_options = "hVvsoet";
static struct option long_options[] = {{"help", no_argument, 0, 'h'},
                                       {"version", no_argument, 0, 'V'},
                                       {"verbose", no_argument, 0, 'v'},
                                       {"script", required_argument, 0, 's'},
                                       {"output", required_argument, 0, 'o'},
                                       {"eval", required_argument, 0, 'e'},
                                       {"help-expr", no_argument, 0, 0},
                                       {"no-interactive", no_argument, 0, 't'},
                                       {0, 0, 0, 0}};

static const char *help_text[] = {
    "Display this help and exit",
    "Display version info and exit",
    "Raise the log level by 1",
    "Load and execute a script",
    "Eval an expression",
    "Show help for expressions",
    "Do not enter interactive mode after executing all scripts",
    NULL};

static const char *arg_name[] = {NULL,   NULL, NULL, "FILE", "FILE",
                                 "EXPR", NULL, NULL, NULL};

static const char *positionals[] = {"INPUT", NULL};

void hog_args_syntax(void) {
  fprintf(stderr, "Usage: %s [OPTION]... [INPUT]\n", progname);
  const struct option *lo = long_options;

  while (lo) {
    lo++;
  }
}

void hog_args_glossary(void) {}

void hog_args_print_help(void) {
  hog_args_syntax();
  fprintf(stderr, "%s\n\n", short_desc);
  hog_args_glossary();
  exit(1); // NOLINT
}

void hog_args_print_version(void) {
  fprintf(stderr, "%s version %d.%d.%d\n", progname, version_major,
          version_minor, version_patch);
}

void hog_args_print_err(void) {
  fprintf(stderr, "Try '%s --help' for more information.\n", progname);
  exit(-1); // NOLINT
}

struct hog_config hog_args_to_config(int argc, char **argv) {
  int option_index = 0;
  int opt = 0;

  struct hog_config cfg = hog_config_init();

  // hanle opts
  while ((opt = getopt_long(argc, argv, short_options, long_options, // NOLINT
                            &option_index)) != -1) {
    switch (opt) {
    case 'h':
    case '?':
      hog_args_print_help();
      break;
    case 'v':
      cfg.log_level++;
      break;
    case 'V':
      hog_args_print_version();
      break;
    default:
      fprintf(stderr, "Unhandeled option: %c\n", opt);
      break;
    }
  }

  // handle positionals
  if (optind < argc) {
    cfg.fin_path = argv[optind++];
    printf("%s\n", cfg.fin_path);
  }

  while (optind < argc) {
    fprintf(stderr, "Superfluous positional argument: %s\n", argv[optind++]);
  }

  hog_log_init(cfg.log_level);
  return cfg;
}
