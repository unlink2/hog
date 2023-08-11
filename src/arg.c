#include "argtable2.h"
#include "arg.h"
#include "libhog/config.h"
#include "libhog/error.h"
#include "libhog/log.h"
#include "libhog/machine.h"
#include "libhog/parser.h"

struct hog_config hog_args_to_config(int argc, char **argv) {
  struct hog_config cfg = hog_config_init();

  struct arg_lit *verb = NULL;
  struct arg_lit *help = NULL;
  struct arg_lit *version = NULL;
  struct arg_file *script = NULL;
  struct arg_file *input = NULL;
  struct arg_file *output = NULL;
  struct arg_str *eval = NULL;
  struct arg_str *main = NULL;

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
      main = arg_str0(NULL, NULL, "ENTRY-POINT", "Select entry point function"),
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
    cfg.log_level = verb->count;
  } else {
    hog_log_init(HOG_LOG_LEVEL_ERROR);
  }

  if (nerrors > 0) {
    arg_print_errors(stdout, end, progname);
    printf("Try '%s --help' for more information.\n", progname);
    exitcode = 1;
    goto exit;
  }

  if (main->count) {
    cfg.main = main->sval[0];
  }

  // TODO: how do we redirect stdin?
  FILE *finput = stdin;

  FILE *foutput = stdout;
  if (output->count) {
    foutput = fopen(output->filename[0], "we");
  }

  FILE *fin = stdin;
  if (input->count) {
    fin = fopen(input->filename[0], "re");
  }

  *cfg.vm = hog_vm_init(HOG_VM_DEFAULT_MEM_SIZE, finput, foutput, fin);

  for (size_t i = 0; i < eval->count; i++) {
    hog_parse_eval(cfg.vm, eval->sval[i]);
    if (hog_err()) {
      goto exit;
    }
  }

  for (size_t i = 0; i < script->count; i++) {
    FILE *f = NULL;

    if (strncmp(script->filename[i], "-", 1) == 0) {
      f = stdin;
    } else {
      f = fopen(script->filename[i], "re");
    }
    if (!f) {
      hog_errno();
      hog_error("No such file '%s'\n", script->filename[i]);
      goto exit;
    }
    hog_parse_from(cfg.vm, f);
    if (hog_err()) {
      goto exit;
    }

    fclose(f);
  }

  // map args to cfg data here

  arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));

  hog_config_build(&cfg);

  return cfg;
exit:
  exit(exitcode); // NOLINT
}
