/************************************************************************************************************************
 Copyright (c) 2016, Imagination Technologies Limited and/or its affiliated group companies.
 All rights reserved.

 Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
 following conditions are met:
     1. Redistributions of source code must retain the above copyright notice, this list of conditions and the
        following disclaimer.
     2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the
        following disclaimer in the documentation and/or other materials provided with the distribution.
     3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote
        products derived from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
 SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
 WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE 
 USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
************************************************************************************************************************/


/* If we use autoconf.  */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef FIX_UNUSED
#define FIX_UNUSED(X) (void) (X) /* avoid warnings for unused params */
#endif


#include "awa-client-delete_cmdline.h"

const char *gengetopt_args_info_purpose = "LWM2M Client Resource Delete Operation";

const char *gengetopt_args_info_usage = "Usage: awa-client-delete [OPTIONS]... [PATHS]...";

const char *gengetopt_args_info_versiontext = "";

const char *gengetopt_args_info_description = "";

const char *gengetopt_args_info_help[] = {
  "  -h, --help                Print help and exit",
  "  -V, --version             Print version and exit",
  "  -v, --verbose             Increase program verbosity  (default=off)",
  "  -d, --debug               Increase program verbosity  (default=off)",
  "  -p, --ipcPort=PORT        Connect to Client IPC port  (default=`12345')",
  "  -a, --ipcAddress=ADDRESS  Connect to Client IPC Address\n                              (default=`127.0.0.1')",
  "\nSpecify one or more object, object instance and resource paths\nto delete in the format \"/O/I/R/i\", separated by spaces.\nFor example:\n\n    /3/0/0\n    /3/0/7/1\n    /4/0/3 /4/0/6\n",
    0
};

typedef enum {ARG_NO
  , ARG_FLAG
  , ARG_STRING
  , ARG_INT
} cmdline_parser_arg_type;

static
void clear_given (struct gengetopt_args_info *args_info);
static
void clear_args (struct gengetopt_args_info *args_info);

static int
cmdline_parser_internal (int argc, char **argv, struct gengetopt_args_info *args_info,
                        struct cmdline_parser_params *params, const char *additional_error);


static char *
gengetopt_strdup (const char *s);

static
void clear_given (struct gengetopt_args_info *args_info)
{
  args_info->help_given = 0 ;
  args_info->version_given = 0 ;
  args_info->verbose_given = 0 ;
  args_info->debug_given = 0 ;
  args_info->ipcPort_given = 0 ;
  args_info->ipcAddress_given = 0 ;
}

static
void clear_args (struct gengetopt_args_info *args_info)
{
  FIX_UNUSED (args_info);
  args_info->verbose_flag = 0;
  args_info->debug_flag = 0;
  args_info->ipcPort_arg = 12345;
  args_info->ipcPort_orig = NULL;
  args_info->ipcAddress_arg = gengetopt_strdup ("127.0.0.1");
  args_info->ipcAddress_orig = NULL;
  
}

static
void init_args_info(struct gengetopt_args_info *args_info)
{


  args_info->help_help = gengetopt_args_info_help[0] ;
  args_info->version_help = gengetopt_args_info_help[1] ;
  args_info->verbose_help = gengetopt_args_info_help[2] ;
  args_info->debug_help = gengetopt_args_info_help[3] ;
  args_info->ipcPort_help = gengetopt_args_info_help[4] ;
  args_info->ipcAddress_help = gengetopt_args_info_help[5] ;
  
}

void
cmdline_parser_print_version (void)
{
  printf ("%s %s\n",
     (strlen(CMDLINE_PARSER_PACKAGE_NAME) ? CMDLINE_PARSER_PACKAGE_NAME : CMDLINE_PARSER_PACKAGE),
     CMDLINE_PARSER_VERSION);

  if (strlen(gengetopt_args_info_versiontext) > 0)
    printf("\n%s\n", gengetopt_args_info_versiontext);
}

static void print_help_common(void) {
  cmdline_parser_print_version ();

  if (strlen(gengetopt_args_info_purpose) > 0)
    printf("\n%s\n", gengetopt_args_info_purpose);

  if (strlen(gengetopt_args_info_usage) > 0)
    printf("\n%s\n", gengetopt_args_info_usage);

  printf("\n");

  if (strlen(gengetopt_args_info_description) > 0)
    printf("%s\n\n", gengetopt_args_info_description);
}

void
cmdline_parser_print_help (void)
{
  int i = 0;
  print_help_common();
  while (gengetopt_args_info_help[i])
    printf("%s\n", gengetopt_args_info_help[i++]);
}

void
cmdline_parser_init (struct gengetopt_args_info *args_info)
{
  clear_given (args_info);
  clear_args (args_info);
  init_args_info (args_info);

  args_info->inputs = 0;
  args_info->inputs_num = 0;
}

void
cmdline_parser_params_init(struct cmdline_parser_params *params)
{
  if (params)
    { 
      params->override = 0;
      params->initialize = 1;
      params->check_required = 1;
      params->check_ambiguity = 0;
      params->print_errors = 1;
    }
}

struct cmdline_parser_params *
cmdline_parser_params_create(void)
{
  struct cmdline_parser_params *params = 
    (struct cmdline_parser_params *)malloc(sizeof(struct cmdline_parser_params));
  cmdline_parser_params_init(params);  
  return params;
}

static void
free_string_field (char **s)
{
  if (*s)
    {
      free (*s);
      *s = 0;
    }
}


static void
cmdline_parser_release (struct gengetopt_args_info *args_info)
{
  unsigned int i;
  free_string_field (&(args_info->ipcPort_orig));
  free_string_field (&(args_info->ipcAddress_arg));
  free_string_field (&(args_info->ipcAddress_orig));
  
  
  for (i = 0; i < args_info->inputs_num; ++i)
    free (args_info->inputs [i]);

  if (args_info->inputs_num)
    free (args_info->inputs);

  clear_given (args_info);
}


static void
write_into_file(FILE *outfile, const char *opt, const char *arg, const char *values[])
{
  FIX_UNUSED (values);
  if (arg) {
    fprintf(outfile, "%s=\"%s\"\n", opt, arg);
  } else {
    fprintf(outfile, "%s\n", opt);
  }
}


int
cmdline_parser_dump(FILE *outfile, struct gengetopt_args_info *args_info)
{
  int i = 0;

  if (!outfile)
    {
      fprintf (stderr, "%s: cannot dump options to stream\n", CMDLINE_PARSER_PACKAGE);
      return EXIT_FAILURE;
    }

  if (args_info->help_given)
    write_into_file(outfile, "help", 0, 0 );
  if (args_info->version_given)
    write_into_file(outfile, "version", 0, 0 );
  if (args_info->verbose_given)
    write_into_file(outfile, "verbose", 0, 0 );
  if (args_info->debug_given)
    write_into_file(outfile, "debug", 0, 0 );
  if (args_info->ipcPort_given)
    write_into_file(outfile, "ipcPort", args_info->ipcPort_orig, 0);
  if (args_info->ipcAddress_given)
    write_into_file(outfile, "ipcAddress", args_info->ipcAddress_orig, 0);
  

  i = EXIT_SUCCESS;
  return i;
}

int
cmdline_parser_file_save(const char *filename, struct gengetopt_args_info *args_info)
{
  FILE *outfile;
  int i = 0;

  outfile = fopen(filename, "w");

  if (!outfile)
    {
      fprintf (stderr, "%s: cannot open file for writing: %s\n", CMDLINE_PARSER_PACKAGE, filename);
      return EXIT_FAILURE;
    }

  i = cmdline_parser_dump(outfile, args_info);
  fclose (outfile);

  return i;
}

void
cmdline_parser_free (struct gengetopt_args_info *args_info)
{
  cmdline_parser_release (args_info);
}

/** @brief replacement of strdup, which is not standard */
char *
gengetopt_strdup (const char *s)
{
  char *result = 0;
  if (!s)
    return result;

  result = (char*)malloc(strlen(s) + 1);
  if (result == (char*)0)
    return (char*)0;
  strcpy(result, s);
  return result;
}

int
cmdline_parser (int argc, char **argv, struct gengetopt_args_info *args_info)
{
  return cmdline_parser2 (argc, argv, args_info, 0, 1, 1);
}

int
cmdline_parser_ext (int argc, char **argv, struct gengetopt_args_info *args_info,
                   struct cmdline_parser_params *params)
{
  int result;
  result = cmdline_parser_internal (argc, argv, args_info, params, 0);

  if (result == EXIT_FAILURE)
    {
      cmdline_parser_free (args_info);
      exit (EXIT_FAILURE);
    }
  
  return result;
}

int
cmdline_parser2 (int argc, char **argv, struct gengetopt_args_info *args_info, int override, int initialize, int check_required)
{
  int result;
  struct cmdline_parser_params params;
  
  params.override = override;
  params.initialize = initialize;
  params.check_required = check_required;
  params.check_ambiguity = 0;
  params.print_errors = 1;

  result = cmdline_parser_internal (argc, argv, args_info, &params, 0);

  if (result == EXIT_FAILURE)
    {
      cmdline_parser_free (args_info);
      exit (EXIT_FAILURE);
    }
  
  return result;
}

int
cmdline_parser_required (struct gengetopt_args_info *args_info, const char *prog_name)
{
  FIX_UNUSED (args_info);
  FIX_UNUSED (prog_name);
  return EXIT_SUCCESS;
}

/*
 * Extracted from the glibc source tree, version 2.3.6
 *
 * Licensed under the GPL as per the whole glibc source tree.
 *
 * This file was modified so that getopt_long can be called
 * many times without risking previous memory to be spoiled.
 *
 * Modified by Andre Noll and Lorenzo Bettini for use in
 * GNU gengetopt generated files.
 *
 */

/* 
 * we must include anything we need since this file is not thought to be
 * inserted in a file already using getopt.h
 *
 * Lorenzo
 */

struct option
{
  const char *name;
  /* has_arg can't be an enum because some compilers complain about
     type mismatches in all the code that assumes it is an int.  */
  int has_arg;
  int *flag;
  int val;
};

/* This version of `getopt' appears to the caller like standard Unix `getopt'
   but it behaves differently for the user, since it allows the user
   to intersperse the options with the other arguments.

   As `getopt' works, it permutes the elements of ARGV so that,
   when it is done, all the options precede everything else.  Thus
   all application programs are extended to handle flexible argument order.
*/
/*
   If the field `flag' is not NULL, it points to a variable that is set
   to the value given in the field `val' when the option is found, but
   left unchanged if the option is not found.

   To have a long-named option do something other than set an `int' to
   a compiled-in constant, such as set a value from `custom_optarg', set the
   option's `flag' field to zero and its `val' field to a nonzero
   value (the equivalent single-letter option character, if there is
   one).  For long options that have a zero `flag' field, `getopt'
   returns the contents of the `val' field.  */

/* Names for the values of the `has_arg' field of `struct option'.  */
#ifndef no_argument
#define no_argument     0
#endif

#ifndef required_argument
#define required_argument   1
#endif

#ifndef optional_argument
#define optional_argument   2
#endif

struct custom_getopt_data {
    /*
     * These have exactly the same meaning as the corresponding global variables,
     * except that they are used for the reentrant versions of getopt.
     */
    int custom_optind;
    int custom_opterr;
    int custom_optopt;
    char *custom_optarg;

    /* True if the internal members have been initialized.  */
    int initialized;

    /*
     * The next char to be scanned in the option-element in which the last option
     * character we returned was found.  This allows us to pick up the scan where
     * we left off.  If this is zero, or a null string, it means resume the scan by
     * advancing to the next ARGV-element.
     */
    char *nextchar;

    /*
     * Describe the part of ARGV that contains non-options that have been skipped.
     * `first_nonopt' is the index in ARGV of the first of them; `last_nonopt' is
     * the index after the last of them.
     */
    int first_nonopt;
    int last_nonopt;
};

/*
 * the variables optarg, optind, opterr and optopt are renamed with
 * the custom_ prefix so that they don't interfere with getopt ones.
 *
 * Moreover they're static so they are visible only from within the
 * file where this very file will be included.
 */

/*
 * For communication from `custom_getopt' to the caller.  When `custom_getopt' finds an
 * option that takes an argument, the argument value is returned here.
 */
static char *custom_optarg;

/*
 * Index in ARGV of the next element to be scanned.  This is used for
 * communication to and from the caller and for communication between
 * successive calls to `custom_getopt'.
 *
 * On entry to `custom_getopt', 1 means this is the first call; initialize.
 *
 * When `custom_getopt' returns -1, this is the index of the first of the non-option
 * elements that the caller should itself scan.
 *
 * Otherwise, `custom_optind' communicates from one call to the next how much of ARGV
 * has been scanned so far.
 *
 * 1003.2 says this must be 1 before any call.
 */
static int custom_optind = 1;

/*
 * Callers store zero here to inhibit the error message for unrecognized
 * options.
 */
static int custom_opterr = 1;

/*
 * Set to an option character which was unrecognized.  This must be initialized
 * on some systems to avoid linking in the system's own getopt implementation.
 */
static int custom_optopt = '?';

/*
 * Exchange two adjacent subsequences of ARGV.  One subsequence is elements
 * [first_nonopt,last_nonopt) which contains all the non-options that have been
 * skipped so far.  The other is elements [last_nonopt,custom_optind), which contains
 * all the options processed since those non-options were skipped.
 * `first_nonopt' and `last_nonopt' are relocated so that they describe the new
 * indices of the non-options in ARGV after they are moved.
 */
static void exchange(char **argv, struct custom_getopt_data *d)
{
    int bottom = d->first_nonopt;
    int middle = d->last_nonopt;
    int top = d->custom_optind;
    char *tem;

    /*
     * Exchange the shorter segment with the far end of the longer segment.
     * That puts the shorter segment into the right place.  It leaves the
     * longer segment in the right place overall, but it consists of two
     * parts that need to be swapped next.
     */
    while (top > middle && middle > bottom) {
        if (top - middle > middle - bottom) {
            /* Bottom segment is the short one.  */
            int len = middle - bottom;
            int i;

            /* Swap it with the top part of the top segment.  */
            for (i = 0; i < len; i++) {
                tem = argv[bottom + i];
                argv[bottom + i] =
                    argv[top - (middle - bottom) + i];
                argv[top - (middle - bottom) + i] = tem;
            }
            /* Exclude the moved bottom segment from further swapping.  */
            top -= len;
        } else {
            /* Top segment is the short one.  */
            int len = top - middle;
            int i;

            /* Swap it with the bottom part of the bottom segment.  */
            for (i = 0; i < len; i++) {
                tem = argv[bottom + i];
                argv[bottom + i] = argv[middle + i];
                argv[middle + i] = tem;
            }
            /* Exclude the moved top segment from further swapping.  */
            bottom += len;
        }
    }
    /* Update records for the slots the non-options now occupy.  */
    d->first_nonopt += (d->custom_optind - d->last_nonopt);
    d->last_nonopt = d->custom_optind;
}

/* Initialize the internal data when the first call is made.  */
static void custom_getopt_initialize(struct custom_getopt_data *d)
{
    /*
     * Start processing options with ARGV-element 1 (since ARGV-element 0
     * is the program name); the sequence of previously skipped non-option
     * ARGV-elements is empty.
     */
    d->first_nonopt = d->last_nonopt = d->custom_optind;
    d->nextchar = NULL;
    d->initialized = 1;
}

#define NONOPTION_P (argv[d->custom_optind][0] != '-' || argv[d->custom_optind][1] == '\0')

/* return: zero: continue, nonzero: return given value to user */
static int shuffle_argv(int argc, char *const *argv,const struct option *longopts,
    struct custom_getopt_data *d)
{
    /*
     * Give FIRST_NONOPT & LAST_NONOPT rational values if CUSTOM_OPTIND has been
     * moved back by the user (who may also have changed the arguments).
     */
    if (d->last_nonopt > d->custom_optind)
        d->last_nonopt = d->custom_optind;
    if (d->first_nonopt > d->custom_optind)
        d->first_nonopt = d->custom_optind;
    /*
     * If we have just processed some options following some
     * non-options, exchange them so that the options come first.
     */
    if (d->first_nonopt != d->last_nonopt &&
            d->last_nonopt != d->custom_optind)
        exchange((char **) argv, d);
    else if (d->last_nonopt != d->custom_optind)
        d->first_nonopt = d->custom_optind;
    /*
     * Skip any additional non-options and extend the range of
     * non-options previously skipped.
     */
    while (d->custom_optind < argc && NONOPTION_P)
        d->custom_optind++;
    d->last_nonopt = d->custom_optind;
    /*
     * The special ARGV-element `--' means premature end of options.  Skip
     * it like a null option, then exchange with previous non-options as if
     * it were an option, then skip everything else like a non-option.
     */
    if (d->custom_optind != argc && !strcmp(argv[d->custom_optind], "--")) {
        d->custom_optind++;
        if (d->first_nonopt != d->last_nonopt
                && d->last_nonopt != d->custom_optind)
            exchange((char **) argv, d);
        else if (d->first_nonopt == d->last_nonopt)
            d->first_nonopt = d->custom_optind;
        d->last_nonopt = argc;
        d->custom_optind = argc;
    }
    /*
     * If we have done all the ARGV-elements, stop the scan and back over
     * any non-options that we skipped and permuted.
     */
    if (d->custom_optind == argc) {
        /*
         * Set the next-arg-index to point at the non-options that we
         * previously skipped, so the caller will digest them.
         */
        if (d->first_nonopt != d->last_nonopt)
            d->custom_optind = d->first_nonopt;
        return -1;
    }
    /*
     * If we have come to a non-option and did not permute it, either stop
     * the scan or describe it to the caller and pass it by.
     */
    if (NONOPTION_P) {
        d->custom_optarg = argv[d->custom_optind++];
        return 1;
    }
    /*
     * We have found another option-ARGV-element. Skip the initial
     * punctuation.
     */
    d->nextchar = (argv[d->custom_optind] + 1 + (longopts != NULL && argv[d->custom_optind][1] == '-'));
    return 0;
}

/*
 * Check whether the ARGV-element is a long option.
 *
 * If there's a long option "fubar" and the ARGV-element is "-fu", consider
 * that an abbreviation of the long option, just like "--fu", and not "-f" with
 * arg "u".
 *
 * This distinction seems to be the most useful approach.
 *
 */
static int check_long_opt(int argc, char *const *argv, const char *optstring,
        const struct option *longopts, int *longind,
        int print_errors, struct custom_getopt_data *d)
{
    char *nameend;
    const struct option *p;
    const struct option *pfound = NULL;
    int exact = 0;
    int ambig = 0;
    int indfound = -1;
    int option_index;

    for (nameend = d->nextchar; *nameend && *nameend != '='; nameend++)
        /* Do nothing.  */ ;

    /* Test all long options for either exact match or abbreviated matches */
    for (p = longopts, option_index = 0; p->name; p++, option_index++)
        if (!strncmp(p->name, d->nextchar, nameend - d->nextchar)) {
            if ((unsigned int) (nameend - d->nextchar)
                    == (unsigned int) strlen(p->name)) {
                /* Exact match found.  */
                pfound = p;
                indfound = option_index;
                exact = 1;
                break;
            } else if (pfound == NULL) {
                /* First nonexact match found.  */
                pfound = p;
                indfound = option_index;
            } else if (pfound->has_arg != p->has_arg
                    || pfound->flag != p->flag
                    || pfound->val != p->val)
                /* Second or later nonexact match found.  */
                ambig = 1;
        }
    if (ambig && !exact) {
        if (print_errors) {
            fprintf(stderr,
                "%s: option `%s' is ambiguous\n",
                argv[0], argv[d->custom_optind]);
        }
        d->nextchar += strlen(d->nextchar);
        d->custom_optind++;
        d->custom_optopt = 0;
        return '?';
    }
    if (pfound) {
        option_index = indfound;
        d->custom_optind++;
        if (*nameend) {
            if (pfound->has_arg != no_argument)
                d->custom_optarg = nameend + 1;
            else {
                if (print_errors) {
                    if (argv[d->custom_optind - 1][1] == '-') {
                        /* --option */
                        fprintf(stderr, "%s: option `--%s' doesn't allow an argument\n",
                            argv[0], pfound->name);
                    } else {
                        /* +option or -option */
                        fprintf(stderr, "%s: option `%c%s' doesn't allow an argument\n",
                            argv[0], argv[d->custom_optind - 1][0], pfound->name);
                    }

                }
                d->nextchar += strlen(d->nextchar);
                d->custom_optopt = pfound->val;
                return '?';
            }
        } else if (pfound->has_arg == required_argument) {
            if (d->custom_optind < argc)
                d->custom_optarg = argv[d->custom_optind++];
            else {
                if (print_errors) {
                    fprintf(stderr,
                        "%s: option `%s' requires an argument\n",
                        argv[0],
                        argv[d->custom_optind - 1]);
                }
                d->nextchar += strlen(d->nextchar);
                d->custom_optopt = pfound->val;
                return optstring[0] == ':' ? ':' : '?';
            }
        }
        d->nextchar += strlen(d->nextchar);
        if (longind != NULL)
            *longind = option_index;
        if (pfound->flag) {
            *(pfound->flag) = pfound->val;
            return 0;
        }
        return pfound->val;
    }
    /*
     * Can't find it as a long option.  If this is not getopt_long_only, or
     * the option starts with '--' or is not a valid short option, then
     * it's an error.  Otherwise interpret it as a short option.
     */
    if (print_errors) {
        if (argv[d->custom_optind][1] == '-') {
            /* --option */
            fprintf(stderr,
                "%s: unrecognized option `--%s'\n",
                argv[0], d->nextchar);
        } else {
            /* +option or -option */
            fprintf(stderr,
                "%s: unrecognized option `%c%s'\n",
                argv[0], argv[d->custom_optind][0],
                d->nextchar);
        }
    }
    d->nextchar = (char *) "";
    d->custom_optind++;
    d->custom_optopt = 0;
    return '?';
}

static int check_short_opt(int argc, char *const *argv, const char *optstring,
        int print_errors, struct custom_getopt_data *d)
{
    char c = *d->nextchar++;
    const char *temp = strchr(optstring, c);

    /* Increment `custom_optind' when we start to process its last character.  */
    if (*d->nextchar == '\0')
        ++d->custom_optind;
    if (!temp || c == ':') {
        if (print_errors)
            fprintf(stderr, "%s: invalid option -- %c\n", argv[0], c);

        d->custom_optopt = c;
        return '?';
    }
    if (temp[1] == ':') {
        if (temp[2] == ':') {
            /* This is an option that accepts an argument optionally.  */
            if (*d->nextchar != '\0') {
                d->custom_optarg = d->nextchar;
                d->custom_optind++;
            } else
                d->custom_optarg = NULL;
            d->nextchar = NULL;
        } else {
            /* This is an option that requires an argument.  */
            if (*d->nextchar != '\0') {
                d->custom_optarg = d->nextchar;
                /*
                 * If we end this ARGV-element by taking the
                 * rest as an arg, we must advance to the next
                 * element now.
                 */
                d->custom_optind++;
            } else if (d->custom_optind == argc) {
                if (print_errors) {
                    fprintf(stderr,
                        "%s: option requires an argument -- %c\n",
                        argv[0], c);
                }
                d->custom_optopt = c;
                if (optstring[0] == ':')
                    c = ':';
                else
                    c = '?';
            } else
                /*
                 * We already incremented `custom_optind' once;
                 * increment it again when taking next ARGV-elt
                 * as argument.
                 */
                d->custom_optarg = argv[d->custom_optind++];
            d->nextchar = NULL;
        }
    }
    return c;
}

/*
 * Scan elements of ARGV for option characters given in OPTSTRING.
 *
 * If an element of ARGV starts with '-', and is not exactly "-" or "--",
 * then it is an option element.  The characters of this element
 * (aside from the initial '-') are option characters.  If `getopt'
 * is called repeatedly, it returns successively each of the option characters
 * from each of the option elements.
 *
 * If `getopt' finds another option character, it returns that character,
 * updating `custom_optind' and `nextchar' so that the next call to `getopt' can
 * resume the scan with the following option character or ARGV-element.
 *
 * If there are no more option characters, `getopt' returns -1.
 * Then `custom_optind' is the index in ARGV of the first ARGV-element
 * that is not an option.  (The ARGV-elements have been permuted
 * so that those that are not options now come last.)
 *
 * OPTSTRING is a string containing the legitimate option characters.
 * If an option character is seen that is not listed in OPTSTRING,
 * return '?' after printing an error message.  If you set `custom_opterr' to
 * zero, the error message is suppressed but we still return '?'.
 *
 * If a char in OPTSTRING is followed by a colon, that means it wants an arg,
 * so the following text in the same ARGV-element, or the text of the following
 * ARGV-element, is returned in `custom_optarg'.  Two colons mean an option that
 * wants an optional arg; if there is text in the current ARGV-element,
 * it is returned in `custom_optarg', otherwise `custom_optarg' is set to zero.
 *
 * If OPTSTRING starts with `-' or `+', it requests different methods of
 * handling the non-option ARGV-elements.
 * See the comments about RETURN_IN_ORDER and REQUIRE_ORDER, above.
 *
 * Long-named options begin with `--' instead of `-'.
 * Their names may be abbreviated as long as the abbreviation is unique
 * or is an exact match for some defined option.  If they have an
 * argument, it follows the option name in the same ARGV-element, separated
 * from the option name by a `=', or else the in next ARGV-element.
 * When `getopt' finds a long-named option, it returns 0 if that option's
 * `flag' field is nonzero, the value of the option's `val' field
 * if the `flag' field is zero.
 *
 * The elements of ARGV aren't really const, because we permute them.
 * But we pretend they're const in the prototype to be compatible
 * with other systems.
 *
 * LONGOPTS is a vector of `struct option' terminated by an
 * element containing a name which is zero.
 *
 * LONGIND returns the index in LONGOPT of the long-named option found.
 * It is only valid when a long-named option has been found by the most
 * recent call.
 *
 * Return the option character from OPTS just read.  Return -1 when there are
 * no more options.  For unrecognized options, or options missing arguments,
 * `custom_optopt' is set to the option letter, and '?' is returned.
 *
 * The OPTS string is a list of characters which are recognized option letters,
 * optionally followed by colons, specifying that that letter takes an
 * argument, to be placed in `custom_optarg'.
 *
 * If a letter in OPTS is followed by two colons, its argument is optional.
 * This behavior is specific to the GNU `getopt'.
 *
 * The argument `--' causes premature termination of argument scanning,
 * explicitly telling `getopt' that there are no more options.  If OPTS begins
 * with `--', then non-option arguments are treated as arguments to the option
 * '\0'.  This behavior is specific to the GNU `getopt'.
 */

static int getopt_internal_r(int argc, char *const *argv, const char *optstring,
        const struct option *longopts, int *longind,
        struct custom_getopt_data *d)
{
    int ret, print_errors = d->custom_opterr;

    if (optstring[0] == ':')
        print_errors = 0;
    if (argc < 1)
        return -1;
    d->custom_optarg = NULL;

    /* 
     * This is a big difference with GNU getopt, since optind == 0
     * means initialization while here 1 means first call.
     */
    if (d->custom_optind == 0 || !d->initialized) {
        if (d->custom_optind == 0)
            d->custom_optind = 1;   /* Don't scan ARGV[0], the program name.  */
        custom_getopt_initialize(d);
    }
    if (d->nextchar == NULL || *d->nextchar == '\0') {
        ret = shuffle_argv(argc, argv, longopts, d);
        if (ret)
            return ret;
    }
    if (longopts && (argv[d->custom_optind][1] == '-' ))
        return check_long_opt(argc, argv, optstring, longopts,
            longind, print_errors, d);
    return check_short_opt(argc, argv, optstring, print_errors, d);
}

static int custom_getopt_internal(int argc, char *const *argv, const char *optstring,
    const struct option *longopts, int *longind)
{
    int result;
    /* Keep a global copy of all internal members of d */
    static struct custom_getopt_data d;

    d.custom_optind = custom_optind;
    d.custom_opterr = custom_opterr;
    result = getopt_internal_r(argc, argv, optstring, longopts,
        longind, &d);
    custom_optind = d.custom_optind;
    custom_optarg = d.custom_optarg;
    custom_optopt = d.custom_optopt;
    return result;
}

static int custom_getopt_long (int argc, char *const *argv, const char *options,
    const struct option *long_options, int *opt_index)
{
    return custom_getopt_internal(argc, argv, options, long_options,
        opt_index);
}


static char *package_name = 0;

/**
 * @brief updates an option
 * @param field the generic pointer to the field to update
 * @param orig_field the pointer to the orig field
 * @param field_given the pointer to the number of occurrence of this option
 * @param prev_given the pointer to the number of occurrence already seen
 * @param value the argument for this option (if null no arg was specified)
 * @param possible_values the possible values for this option (if specified)
 * @param default_value the default value (in case the option only accepts fixed values)
 * @param arg_type the type of this option
 * @param check_ambiguity @see cmdline_parser_params.check_ambiguity
 * @param override @see cmdline_parser_params.override
 * @param no_free whether to free a possible previous value
 * @param multiple_option whether this is a multiple option
 * @param long_opt the corresponding long option
 * @param short_opt the corresponding short option (or '-' if none)
 * @param additional_error possible further error specification
 */
static
int update_arg(void *field, char **orig_field,
               unsigned int *field_given, unsigned int *prev_given, 
               char *value, const char *possible_values[],
               const char *default_value,
               cmdline_parser_arg_type arg_type,
               int check_ambiguity, int override,
               int no_free, int multiple_option,
               const char *long_opt, char short_opt,
               const char *additional_error)
{
  char *stop_char = 0;
  const char *val = value;
  int found;
  char **string_field;
  FIX_UNUSED (field);

  stop_char = 0;
  found = 0;

  if (!multiple_option && prev_given && (*prev_given || (check_ambiguity && *field_given)))
    {
      if (short_opt != '-')
        fprintf (stderr, "%s: `--%s' (`-%c') option given more than once%s\n", 
               package_name, long_opt, short_opt,
               (additional_error ? additional_error : ""));
      else
        fprintf (stderr, "%s: `--%s' option given more than once%s\n", 
               package_name, long_opt,
               (additional_error ? additional_error : ""));
      return 1; /* failure */
    }

  FIX_UNUSED (default_value);
    
  if (field_given && *field_given && ! override)
    return 0;
  if (prev_given)
    (*prev_given)++;
  if (field_given)
    (*field_given)++;
  if (possible_values)
    val = possible_values[found];

  switch(arg_type) {
  case ARG_FLAG:
    *((int *)field) = !*((int *)field);
    break;
  case ARG_INT:
    if (val) *((int *)field) = strtol (val, &stop_char, 0);
    break;
  case ARG_STRING:
    if (val) {
      string_field = (char **)field;
      if (!no_free && *string_field)
        free (*string_field); /* free previous string */
      *string_field = gengetopt_strdup (val);
    }
    break;
  default:
    break;
  };

  /* check numeric conversion */
  switch(arg_type) {
  case ARG_INT:
    if (val && !(stop_char && *stop_char == '\0')) {
      fprintf(stderr, "%s: invalid numeric value: %s\n", package_name, val);
      return 1; /* failure */
    }
    break;
  default:
    ;
  };

  /* store the original value */
  switch(arg_type) {
  case ARG_NO:
  case ARG_FLAG:
    break;
  default:
    if (value && orig_field) {
      if (no_free) {
        *orig_field = value;
      } else {
        if (*orig_field)
          free (*orig_field); /* free previous string */
        *orig_field = gengetopt_strdup (value);
      }
    }
  };

  return 0; /* OK */
}


int
cmdline_parser_internal (
  int argc, char **argv, struct gengetopt_args_info *args_info,
                        struct cmdline_parser_params *params, const char *additional_error)
{
  int c;    /* Character of the parsed option.  */

  int error_occurred = 0;
  struct gengetopt_args_info local_args_info;
  
  int override;
  int initialize;
  int check_required;
  int check_ambiguity;

  char *optarg;
  int optind;
  int opterr;
  int optopt;
  
  package_name = argv[0];
  
  override = params->override;
  initialize = params->initialize;
  check_required = params->check_required;
  check_ambiguity = params->check_ambiguity;

  if (initialize)
    cmdline_parser_init (args_info);

  cmdline_parser_init (&local_args_info);

  optarg = 0;
  optind = 0;
  opterr = params->print_errors;
  optopt = '?';

  while (1)
    {
      int option_index = 0;

      static struct option long_options[] = {
        { "help",   0, NULL, 'h' },
        { "version",    0, NULL, 'V' },
        { "verbose",    0, NULL, 'v' },
        { "debug",  0, NULL, 'd' },
        { "ipcPort",    1, NULL, 'p' },
        { "ipcAddress", 1, NULL, 'a' },
        { 0,  0, 0, 0 }
      };

      custom_optarg = optarg;
      custom_optind = optind;
      custom_opterr = opterr;
      custom_optopt = optopt;

      c = custom_getopt_long (argc, argv, "hVvdp:a:", long_options, &option_index);

      optarg = custom_optarg;
      optind = custom_optind;
      opterr = custom_opterr;
      optopt = custom_optopt;

      if (c == -1) break;   /* Exit from `while (1)' loop.  */

      switch (c)
        {
        case 'h':   /* Print help and exit.  */
          cmdline_parser_print_help ();
          cmdline_parser_free (&local_args_info);
          exit (EXIT_SUCCESS);

        case 'V':   /* Print version and exit.  */
          cmdline_parser_print_version ();
          cmdline_parser_free (&local_args_info);
          exit (EXIT_SUCCESS);

        case 'v':   /* Increase program verbosity.  */
        
        
          if (update_arg((void *)&(args_info->verbose_flag), 0, &(args_info->verbose_given),
              &(local_args_info.verbose_given), optarg, 0, 0, ARG_FLAG,
              check_ambiguity, override, 1, 0, "verbose", 'v',
              additional_error))
            goto failure;
        
          break;
        case 'd':   /* Increase program verbosity.  */
        
        
          if (update_arg((void *)&(args_info->debug_flag), 0, &(args_info->debug_given),
              &(local_args_info.debug_given), optarg, 0, 0, ARG_FLAG,
              check_ambiguity, override, 1, 0, "debug", 'd',
              additional_error))
            goto failure;
        
          break;
        case 'p':   /* Connect to Client IPC port.  */
        
        
          if (update_arg( (void *)&(args_info->ipcPort_arg), 
               &(args_info->ipcPort_orig), &(args_info->ipcPort_given),
              &(local_args_info.ipcPort_given), optarg, 0, "12345", ARG_INT,
              check_ambiguity, override, 0, 0,
              "ipcPort", 'p',
              additional_error))
            goto failure;
        
          break;
        case 'a':   /* Connect to Client IPC Address.  */
        
        
          if (update_arg( (void *)&(args_info->ipcAddress_arg), 
               &(args_info->ipcAddress_orig), &(args_info->ipcAddress_given),
              &(local_args_info.ipcAddress_given), optarg, 0, "127.0.0.1", ARG_STRING,
              check_ambiguity, override, 0, 0,
              "ipcAddress", 'a',
              additional_error))
            goto failure;
        
          break;

        case 0: /* Long option with no short option */
        case '?':   /* Invalid option.  */
          /* `getopt_long' already printed an error message.  */
          goto failure;

        default:    /* bug: option not considered.  */
          fprintf (stderr, "%s: option unknown: %c%s\n", CMDLINE_PARSER_PACKAGE, c, (additional_error ? additional_error : ""));
          abort ();
        } /* switch */
    } /* while */




  cmdline_parser_release (&local_args_info);

  if ( error_occurred )
    return (EXIT_FAILURE);

  if (optind < argc)
    {
      int i = 0 ;
      int found_prog_name = 0;
      /* whether program name, i.e., argv[0], is in the remaining args
         (this may happen with some implementations of getopt,
          but surely not with the one included by gengetopt) */


      args_info->inputs_num = argc - optind - found_prog_name;
      args_info->inputs =
        (char **)(malloc ((args_info->inputs_num)*sizeof(char *))) ;
      while (optind < argc)
        args_info->inputs[ i++ ] = gengetopt_strdup (argv[optind++]) ;
    }

  return 0;

failure:
  
  cmdline_parser_release (&local_args_info);
  return (EXIT_FAILURE);
}
