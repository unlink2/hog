#include "argtable2.h"
#include "arg.h"
#include "libhog/config.h"
#include "libhog/log.h"

struct hog_config hog_args_to_config(int argc, char **argv) {
  struct arg_lit *verb = NULL;
  struct arg_lit *help = NULL;
  struct arg_lit *version = NULL;
  struct arg_file *script = NULL;
  struct arg_file *input = NULL;
  struct arg_file *output = NULL;
  struct arg_str *eval = NULL;

  struct arg_lit *help_expr = NULL;
  struct arg_lit *no_interactive = NULL;

  // arg end stores errors
  struct arg_end *end = NULL;

  void *argtable[] = {
      help = arg_litn(NULL, "help", 0, 1, "display this help and exit"),
      version =
          arg_litn(NULL, "version", 0, 1, "display version info and exit"),
      verb = arg_litn("v", "verbose", 0, HOG_LOG_LEVEL_DBG, "Verbose output"),
      script = arg_filen("s", "script", "SCRIPT", 0, 256,
                         "Load and execute a script"),
      output = arg_file0("o", "output", "FILE", "Output file"),
      eval = arg_strn("e", "eval", "EXPR", 0, 1024, "Eval an expression"),
      help_expr =
          arg_lit0(NULL, "help-expr", "Show help for expressions syntax"),
      no_interactive = arg_lit0(
          "t", "no-interactive",
          "Do not drop to interactive mode after executing all scripts"),

      input = arg_file0(NULL, NULL, "FILE", "Input file"),
      end = arg_end(20),
  };

  // output params
  char progname[] = "hog";
  char short_desc[] = "";

  // version info
  int version_major = 0;
  int version_minor = 0;
  int version_patch = 1;

  int nerrors = arg_parse(argc, argv, argtable);
  int exitcode = 0;

  if (help->count > 0) {
    printf("Usage: %s", progname);
    arg_print_syntax(stdout, argtable, "\n");
    printf("%s\n\n", short_desc);
    arg_print_glossary(stdout, argtable, "  %-25s %s\n");
    goto exit;
  }

  if (version->count) {
    printf("%s version %d.%d.%d\n", progname, version_major, version_minor,
           version_patch);
    goto exit;
  }

  if (verb->count > 0) {
    hog_log_init(verb->count);
  } else {
    hog_log_init(HOG_LOG_LEVEL_ERROR);
  }

  if (nerrors > 0) {
    arg_print_errors(stdout, end, progname);
    printf("Try '%s --help' for more information.\n", progname);
    exitcode = 1;
    goto exit;
  }

  struct hog_config cfg = hog_config_init();

  // map args to cfg data here

  arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));

  return cfg;
exit:
  exit(exitcode); // NOLINT
}
