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


#include "awa-client-define_cmdline.h"

const char *gengetopt_args_info_purpose = "LWM2M Client Object Definition Operation";

const char *gengetopt_args_info_usage = "Usage: awa-client-define [OPTIONS]... [PATHS]...";

const char *gengetopt_args_info_versiontext = "";

const char *gengetopt_args_info_description = "";

const char *gengetopt_args_info_help[] = {
  "  -h, --help                    Print help and exit",
  "  -V, --version                 Print version and exit",
  "  -v, --verbose                 Increase program verbosity  (default=off)",
  "  -d, --debug                   Increase program verbosity  (default=off)",
  "  -a, --ipcAddress=ADDRESS      Connect to Client IPC Address\n                                  (default=`127.0.0.1')",
  "  -p, --ipcPort=PORT            Connect to Client IPC port  (default=`12345')",
  "\n Mode: XML Definition",
  "  -x, --xmlFile=FILENAME        Load XML Definition file",
  "\n Mode: Cmdline Definition",
  "  -o, --objectID=ID             Object ID",
  "  -j, --objectName=NAME         Object name",
  "  -m, --objectMandatory         Object is required or optional  (default=off)",
  "  -y, --objectInstances=TYPE    Object supports single or multiple instances\n                                  (possible values=\"single\", \"multiple\"\n                                  default=`single')",
  "  -r, --resourceID=ID           Resource ID",
  "  -n, --resourceName=NAME       Resource Name",
  "  -t, --resourceType=TYPE       Resource Type  (possible values=\"opaque\",\n                                  \"integer\", \"float\", \"boolean\",\n                                  \"string\", \"time\", \"objlink\", \"none\")",
  "  -u, --resourceInstances=VALUE Resource supports single or multiple instances\n                                  (possible values=\"single\", \"multiple\")",
  "  -q, --resourceRequired=VALUE  Resource is required or optional  (possible\n                                  values=\"optional\", \"mandatory\")",
  "  -k, --resourceOperations=VALUE\n                                Resource Operation  (possible values=\"r\",\n                                  \"w\", \"e\", \"rw\")",
    0
};

typedef enum {ARG_NO
  , ARG_FLAG
  , ARG_STRING
  , ARG_INT
  , ARG_ENUM
} cmdline_parser_arg_type;

static
void clear_given (struct gengetopt_args_info *args_info);
static
void clear_args (struct gengetopt_args_info *args_info);

static int
cmdline_parser_internal (int argc, char **argv, struct gengetopt_args_info *args_info,
                        struct cmdline_parser_params *params, const char *additional_error);

static int
cmdline_parser_required2 (struct gengetopt_args_info *args_info, const char *prog_name, const char *additional_error);

const char *cmdline_parser_objectInstances_values[] = {"single", "multiple", 0}; /*< Possible values for objectInstances. */
const char *cmdline_parser_resourceType_values[] = {"opaque", "integer", "float", "boolean", "string", "time", "objlink", "none", 0}; /*< Possible values for resourceType. */
const char *cmdline_parser_resourceInstances_values[] = {"single", "multiple", 0}; /*< Possible values for resourceInstances. */
const char *cmdline_parser_resourceRequired_values[] = {"optional", "mandatory", 0}; /*< Possible values for resourceRequired. */
const char *cmdline_parser_resourceOperations_values[] = {"r", "w", "e", "rw", 0}; /*< Possible values for resourceOperations. */

static char *
gengetopt_strdup (const char *s);

static
void clear_given (struct gengetopt_args_info *args_info)
{
  args_info->help_given = 0 ;
  args_info->version_given = 0 ;
  args_info->verbose_given = 0 ;
  args_info->debug_given = 0 ;
  args_info->ipcAddress_given = 0 ;
  args_info->ipcPort_given = 0 ;
  args_info->xmlFile_given = 0 ;
  args_info->objectID_given = 0 ;
  args_info->objectName_given = 0 ;
  args_info->objectMandatory_given = 0 ;
  args_info->objectInstances_given = 0 ;
  args_info->resourceID_given = 0 ;
  args_info->resourceName_given = 0 ;
  args_info->resourceType_given = 0 ;
  args_info->resourceInstances_given = 0 ;
  args_info->resourceRequired_given = 0 ;
  args_info->resourceOperations_given = 0 ;
  args_info->Cmdline_Definition_mode_counter = 0 ;
  args_info->XML_Definition_mode_counter = 0 ;
}

static
void clear_args (struct gengetopt_args_info *args_info)
{
  FIX_UNUSED (args_info);
  args_info->verbose_flag = 0;
  args_info->debug_flag = 0;
  args_info->ipcAddress_arg = gengetopt_strdup ("127.0.0.1");
  args_info->ipcAddress_orig = NULL;
  args_info->ipcPort_arg = 12345;
  args_info->ipcPort_orig = NULL;
  args_info->xmlFile_arg = NULL;
  args_info->xmlFile_orig = NULL;
  args_info->objectID_orig = NULL;
  args_info->objectName_arg = NULL;
  args_info->objectName_orig = NULL;
  args_info->objectMandatory_flag = 0;
  args_info->objectInstances_arg = objectInstances_arg_single;
  args_info->objectInstances_orig = NULL;
  args_info->resourceID_arg = NULL;
  args_info->resourceID_orig = NULL;
  args_info->resourceName_arg = NULL;
  args_info->resourceName_orig = NULL;
  args_info->resourceType_arg = NULL;
  args_info->resourceType_orig = NULL;
  args_info->resourceInstances_arg = NULL;
  args_info->resourceInstances_orig = NULL;
  args_info->resourceRequired_arg = NULL;
  args_info->resourceRequired_orig = NULL;
  args_info->resourceOperations_arg = NULL;
  args_info->resourceOperations_orig = NULL;
  
}

static
void init_args_info(struct gengetopt_args_info *args_info)
{


  args_info->help_help = gengetopt_args_info_help[0] ;
  args_info->version_help = gengetopt_args_info_help[1] ;
  args_info->verbose_help = gengetopt_args_info_help[2] ;
  args_info->debug_help = gengetopt_args_info_help[3] ;
  args_info->ipcAddress_help = gengetopt_args_info_help[4] ;
  args_info->ipcPort_help = gengetopt_args_info_help[5] ;
  args_info->xmlFile_help = gengetopt_args_info_help[7] ;
  args_info->objectID_help = gengetopt_args_info_help[9] ;
  args_info->objectName_help = gengetopt_args_info_help[10] ;
  args_info->objectMandatory_help = gengetopt_args_info_help[11] ;
  args_info->objectInstances_help = gengetopt_args_info_help[12] ;
  args_info->resourceID_help = gengetopt_args_info_help[13] ;
  args_info->resourceID_min = 1;
  args_info->resourceID_max = 0;
  args_info->resourceName_help = gengetopt_args_info_help[14] ;
  args_info->resourceName_min = 1;
  args_info->resourceName_max = 0;
  args_info->resourceType_help = gengetopt_args_info_help[15] ;
  args_info->resourceType_min = 1;
  args_info->resourceType_max = 0;
  args_info->resourceInstances_help = gengetopt_args_info_help[16] ;
  args_info->resourceInstances_min = 1;
  args_info->resourceInstances_max = 0;
  args_info->resourceRequired_help = gengetopt_args_info_help[17] ;
  args_info->resourceRequired_min = 1;
  args_info->resourceRequired_max = 0;
  args_info->resourceOperations_help = gengetopt_args_info_help[18] ;
  args_info->resourceOperations_min = 1;
  args_info->resourceOperations_max = 0;
  
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

/** @brief generic value variable */
union generic_value {
    int int_arg;
    char *string_arg;
    const char *default_string_arg;
};

/** @brief holds temporary values for multiple options */
struct generic_list
{
  union generic_value arg;
  char *orig;
  struct generic_list *next;
};

/**
 * @brief add a node at the head of the list 
 */
static void add_node(struct generic_list **list) {
  struct generic_list *new_node = (struct generic_list *) malloc (sizeof (struct generic_list));
  new_node->next = *list;
  *list = new_node;
  new_node->arg.string_arg = 0;
  new_node->orig = 0;
}

/**
 * The passed arg parameter is NOT set to 0 from this function
 */
static void
free_multiple_field(unsigned int len, void *arg, char ***orig)
{
  unsigned int i;
  if (arg) {
    for (i = 0; i < len; ++i)
      {
        free_string_field(&((*orig)[i]));
      }

    free (arg);
    free (*orig);
    *orig = 0;
  }
}

static void
free_multiple_string_field(unsigned int len, char ***arg, char ***orig)
{
  unsigned int i;
  if (*arg) {
    for (i = 0; i < len; ++i)
      {
        free_string_field(&((*arg)[i]));
        free_string_field(&((*orig)[i]));
      }
    free_string_field(&((*arg)[0])); /* free default string */

    free (*arg);
    *arg = 0;
    free (*orig);
    *orig = 0;
  }
}

static void
cmdline_parser_release (struct gengetopt_args_info *args_info)
{
  unsigned int i;
  free_string_field (&(args_info->ipcAddress_arg));
  free_string_field (&(args_info->ipcAddress_orig));
  free_string_field (&(args_info->ipcPort_orig));
  free_string_field (&(args_info->xmlFile_arg));
  free_string_field (&(args_info->xmlFile_orig));
  free_string_field (&(args_info->objectID_orig));
  free_string_field (&(args_info->objectName_arg));
  free_string_field (&(args_info->objectName_orig));
  free_string_field (&(args_info->objectInstances_orig));
  free_multiple_field (args_info->resourceID_given, (void *)(args_info->resourceID_arg), &(args_info->resourceID_orig));
  args_info->resourceID_arg = 0;
  free_multiple_string_field (args_info->resourceName_given, &(args_info->resourceName_arg), &(args_info->resourceName_orig));
  free_multiple_field (args_info->resourceType_given, (void *)(args_info->resourceType_arg), &(args_info->resourceType_orig));
  args_info->resourceType_arg = 0;
  free_multiple_field (args_info->resourceInstances_given, (void *)(args_info->resourceInstances_arg), &(args_info->resourceInstances_orig));
  args_info->resourceInstances_arg = 0;
  free_multiple_field (args_info->resourceRequired_given, (void *)(args_info->resourceRequired_arg), &(args_info->resourceRequired_orig));
  args_info->resourceRequired_arg = 0;
  free_multiple_field (args_info->resourceOperations_given, (void *)(args_info->resourceOperations_arg), &(args_info->resourceOperations_orig));
  args_info->resourceOperations_arg = 0;
  
  
  for (i = 0; i < args_info->inputs_num; ++i)
    free (args_info->inputs [i]);

  if (args_info->inputs_num)
    free (args_info->inputs);

  clear_given (args_info);
}

/**
 * @param val the value to check
 * @param values the possible values
 * @return the index of the matched value:
 * -1 if no value matched,
 * -2 if more than one value has matched
 */
static int
check_possible_values(const char *val, const char *values[])
{
  int i, found, last;
  size_t len;

  if (!val)   /* otherwise strlen() crashes below */
    return -1; /* -1 means no argument for the option */

  found = last = 0;

  for (i = 0, len = strlen(val); values[i]; ++i)
    {
      if (strncmp(val, values[i], len) == 0)
        {
          ++found;
          last = i;
          if (strlen(values[i]) == len)
            return i; /* exact macth no need to check more */
        }
    }

  if (found == 1) /* one match: OK */
    return last;

  return (found ? -2 : -1); /* return many values or none matched */
}


static void
write_into_file(FILE *outfile, const char *opt, const char *arg, const char *values[])
{
  int found = -1;
  if (arg) {
    if (values) {
      found = check_possible_values(arg, values);      
    }
    if (found >= 0)
      fprintf(outfile, "%s=\"%s\" # %s\n", opt, arg, values[found]);
    else
      fprintf(outfile, "%s=\"%s\"\n", opt, arg);
  } else {
    fprintf(outfile, "%s\n", opt);
  }
}

static void
write_multiple_into_file(FILE *outfile, int len, const char *opt, char **arg, const char *values[])
{
  int i;
  
  for (i = 0; i < len; ++i)
    write_into_file(outfile, opt, (arg ? arg[i] : 0), values);
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
  if (args_info->ipcAddress_given)
    write_into_file(outfile, "ipcAddress", args_info->ipcAddress_orig, 0);
  if (args_info->ipcPort_given)
    write_into_file(outfile, "ipcPort", args_info->ipcPort_orig, 0);
  if (args_info->xmlFile_given)
    write_into_file(outfile, "xmlFile", args_info->xmlFile_orig, 0);
  if (args_info->objectID_given)
    write_into_file(outfile, "objectID", args_info->objectID_orig, 0);
  if (args_info->objectName_given)
    write_into_file(outfile, "objectName", args_info->objectName_orig, 0);
  if (args_info->objectMandatory_given)
    write_into_file(outfile, "objectMandatory", 0, 0 );
  if (args_info->objectInstances_given)
    write_into_file(outfile, "objectInstances", args_info->objectInstances_orig, cmdline_parser_objectInstances_values);
  write_multiple_into_file(outfile, args_info->resourceID_given, "resourceID", args_info->resourceID_orig, 0);
  write_multiple_into_file(outfile, args_info->resourceName_given, "resourceName", args_info->resourceName_orig, 0);
  write_multiple_into_file(outfile, args_info->resourceType_given, "resourceType", args_info->resourceType_orig, cmdline_parser_resourceType_values);
  write_multiple_into_file(outfile, args_info->resourceInstances_given, "resourceInstances", args_info->resourceInstances_orig, cmdline_parser_resourceInstances_values);
  write_multiple_into_file(outfile, args_info->resourceRequired_given, "resourceRequired", args_info->resourceRequired_orig, cmdline_parser_resourceRequired_values);
  write_multiple_into_file(outfile, args_info->resourceOperations_given, "resourceOperations", args_info->resourceOperations_orig, cmdline_parser_resourceOperations_values);
  

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

static char *
get_multiple_arg_token(const char *arg)
{
  const char *tok;
  char *ret;
  size_t len, num_of_escape, i, j;

  if (!arg)
    return 0;

  tok = strchr (arg, ',');
  num_of_escape = 0;

  /* make sure it is not escaped */
  while (tok)
    {
      if (*(tok-1) == '\\')
        {
          /* find the next one */
          tok = strchr (tok+1, ',');
          ++num_of_escape;
        }
      else
        break;
    }

  if (tok)
    len = (size_t)(tok - arg + 1);
  else
    len = strlen (arg) + 1;

  len -= num_of_escape;

  ret = (char *) malloc (len);

  i = 0;
  j = 0;
  while (arg[i] && (j < len-1))
    {
      if (arg[i] == '\\' && 
      arg[ i + 1 ] && 
      arg[ i + 1 ] == ',')
        ++i;

      ret[j++] = arg[i++];
    }

  ret[len-1] = '\0';

  return ret;
}

static const char *
get_multiple_arg_token_next(const char *arg)
{
  const char *tok;

  if (!arg)
    return 0;

  tok = strchr (arg, ',');

  /* make sure it is not escaped */
  while (tok)
    {
      if (*(tok-1) == '\\')
        {
          /* find the next one */
          tok = strchr (tok+1, ',');
        }
      else
        break;
    }

  if (! tok || strlen(tok) == 1)
    return 0;

  return tok+1;
}

static int
check_multiple_option_occurrences(const char *prog_name, unsigned int option_given, unsigned int min, unsigned int max, const char *option_desc);

int
check_multiple_option_occurrences(const char *prog_name, unsigned int option_given, unsigned int min, unsigned int max, const char *option_desc)
{
  int error_occurred = 0;

  if (option_given && (min > 0 || max > 0))
    {
      if (min > 0 && max > 0)
        {
          if (min == max)
            {
              /* specific occurrences */
              if (option_given != (unsigned int) min)
                {
                  fprintf (stderr, "%s: %s option occurrences must be %d\n",
                    prog_name, option_desc, min);
                  error_occurred = 1;
                }
            }
          else if (option_given < (unsigned int) min
                || option_given > (unsigned int) max)
            {
              /* range occurrences */
              fprintf (stderr, "%s: %s option occurrences must be between %d and %d\n",
                prog_name, option_desc, min, max);
              error_occurred = 1;
            }
        }
      else if (min > 0)
        {
          /* at least check */
          if (option_given < min)
            {
              fprintf (stderr, "%s: %s option occurrences must be at least %d\n",
                prog_name, option_desc, min);
              error_occurred = 1;
            }
        }
      else if (max > 0)
        {
          /* at most check */
          if (option_given > max)
            {
              fprintf (stderr, "%s: %s option occurrences must be at most %d\n",
                prog_name, option_desc, max);
              error_occurred = 1;
            }
        }
    }
    
  return error_occurred;
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
  int result = EXIT_SUCCESS;

  if (cmdline_parser_required2(args_info, prog_name, 0) > 0)
    result = EXIT_FAILURE;

  if (result == EXIT_FAILURE)
    {
      cmdline_parser_free (args_info);
      exit (EXIT_FAILURE);
    }
  
  return result;
}

int
cmdline_parser_required2 (struct gengetopt_args_info *args_info, const char *prog_name, const char *additional_error)
{
  int error_occurred = 0;
  FIX_UNUSED (additional_error);

  /* checks for required options */
  if (args_info->XML_Definition_mode_counter && ! args_info->xmlFile_given)
    {
      fprintf (stderr, "%s: '--xmlFile' ('-x') option required%s\n", prog_name, (additional_error ? additional_error : ""));
      error_occurred = 1;
    }
  
  if (args_info->Cmdline_Definition_mode_counter && ! args_info->objectID_given)
    {
      fprintf (stderr, "%s: '--objectID' ('-o') option required%s\n", prog_name, (additional_error ? additional_error : ""));
      error_occurred = 1;
    }
  
  if (args_info->Cmdline_Definition_mode_counter && check_multiple_option_occurrences(prog_name, args_info->resourceID_given, args_info->resourceID_min, args_info->resourceID_max, "'--resourceID' ('-r')"))
     error_occurred = 1;
  
  if (args_info->Cmdline_Definition_mode_counter && check_multiple_option_occurrences(prog_name, args_info->resourceName_given, args_info->resourceName_min, args_info->resourceName_max, "'--resourceName' ('-n')"))
     error_occurred = 1;
  
  if (args_info->Cmdline_Definition_mode_counter && check_multiple_option_occurrences(prog_name, args_info->resourceType_given, args_info->resourceType_min, args_info->resourceType_max, "'--resourceType' ('-t')"))
     error_occurred = 1;
  
  if (args_info->Cmdline_Definition_mode_counter && check_multiple_option_occurrences(prog_name, args_info->resourceInstances_given, args_info->resourceInstances_min, args_info->resourceInstances_max, "'--resourceInstances' ('-u')"))
     error_occurred = 1;
  
  if (args_info->Cmdline_Definition_mode_counter && check_multiple_option_occurrences(prog_name, args_info->resourceRequired_given, args_info->resourceRequired_min, args_info->resourceRequired_max, "'--resourceRequired' ('-q')"))
     error_occurred = 1;
  
  if (args_info->Cmdline_Definition_mode_counter && check_multiple_option_occurrences(prog_name, args_info->resourceOperations_given, args_info->resourceOperations_min, args_info->resourceOperations_max, "'--resourceOperations' ('-k')"))
     error_occurred = 1;
  
  
  /* checks for dependences among options */

  return error_occurred;
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

  if (possible_values && (found = check_possible_values((value ? value : default_value), possible_values)) < 0)
    {
      if (short_opt != '-')
        fprintf (stderr, "%s: %s argument, \"%s\", for option `--%s' (`-%c')%s\n", 
          package_name, (found == -2) ? "ambiguous" : "invalid", value, long_opt, short_opt,
          (additional_error ? additional_error : ""));
      else
        fprintf (stderr, "%s: %s argument, \"%s\", for option `--%s'%s\n", 
          package_name, (found == -2) ? "ambiguous" : "invalid", value, long_opt,
          (additional_error ? additional_error : ""));
      return 1; /* failure */
    }
    
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
  case ARG_ENUM:
    if (val) *((int *)field) = found;
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

/**
 * @brief store information about a multiple option in a temporary list
 * @param list where to (temporarily) store multiple options
 */
static
int update_multiple_arg_temp(struct generic_list **list,
               unsigned int *prev_given, const char *val,
               const char *possible_values[], const char *default_value,
               cmdline_parser_arg_type arg_type,
               const char *long_opt, char short_opt,
               const char *additional_error)
{
  /* store single arguments */
  char *multi_token;
  const char *multi_next;

  if (arg_type == ARG_NO) {
    (*prev_given)++;
    return 0; /* OK */
  }

  multi_token = get_multiple_arg_token(val);
  multi_next = get_multiple_arg_token_next (val);

  while (1)
    {
      add_node (list);
      if (update_arg((void *)&((*list)->arg), &((*list)->orig), 0,
          prev_given, multi_token, possible_values, default_value, 
          arg_type, 0, 1, 1, 1, long_opt, short_opt, additional_error)) {
        if (multi_token) free(multi_token);
        return 1; /* failure */
      }

      if (multi_next)
        {
          multi_token = get_multiple_arg_token(multi_next);
          multi_next = get_multiple_arg_token_next (multi_next);
        }
      else
        break;
    }

  return 0; /* OK */
}

/**
 * @brief free the passed list (including possible string argument)
 */
static
void free_list(struct generic_list *list, short string_arg)
{
  if (list) {
    struct generic_list *tmp;
    while (list)
      {
        tmp = list;
        if (string_arg && list->arg.string_arg)
          free (list->arg.string_arg);
        if (list->orig)
          free (list->orig);
        list = list->next;
        free (tmp);
      }
  }
}

/**
 * @brief updates a multiple option starting from the passed list
 */
static
void update_multiple_arg(void *field, char ***orig_field,
               unsigned int field_given, unsigned int prev_given, union generic_value *default_value,
               cmdline_parser_arg_type arg_type,
               struct generic_list *list)
{
  int i;
  struct generic_list *tmp;

  if (prev_given && list) {
    *orig_field = (char **) realloc (*orig_field, (field_given + prev_given) * sizeof (char *));

    switch(arg_type) {
    case ARG_INT:
    case ARG_ENUM:
      *((int **)field) = (int *)realloc (*((int **)field), (field_given + prev_given) * sizeof (int)); break;
    case ARG_STRING:
      *((char ***)field) = (char **)realloc (*((char ***)field), (field_given + prev_given) * sizeof (char *)); break;
    default:
      break;
    };
    
    for (i = (prev_given - 1); i >= 0; --i)
      {
        tmp = list;
        
        switch(arg_type) {
        case ARG_INT:
          (*((int **)field))[i + field_given] = tmp->arg.int_arg; break;
        case ARG_ENUM:
          (*((int **)field))[i + field_given] = tmp->arg.int_arg; break;
        case ARG_STRING:
          (*((char ***)field))[i + field_given] = tmp->arg.string_arg; break;
        default:
          break;
        }        
        (*orig_field) [i + field_given] = list->orig;
        list = list->next;
        free (tmp);
      }
  } else { /* set the default value */
    if (default_value && ! field_given) {
      switch(arg_type) {
      case ARG_INT:
      case ARG_ENUM:
        if (! *((int **)field)) {
          *((int **)field) = (int *)malloc (sizeof (int));
          (*((int **)field))[0] = default_value->int_arg; 
        }
        break;
      case ARG_STRING:
        if (! *((char ***)field)) {
          *((char ***)field) = (char **)malloc (sizeof (char *));
          (*((char ***)field))[0] = gengetopt_strdup(default_value->string_arg);
        }
        break;
      default: break;
      }
      if (!(*orig_field)) {
        *orig_field = (char **) malloc (sizeof (char *));
        (*orig_field)[0] = 0;
      }
    }
  }
}

static int check_modes(
  int given1[], const char *options1[],
                       int given2[], const char *options2[])
{
  int i = 0, j = 0, errors = 0;
  
  while (given1[i] >= 0) {
    if (given1[i]) {
      while (given2[j] >= 0) {
        if (given2[j]) {
          ++errors;
          fprintf(stderr, "%s: option %s conflicts with option %s\n",
                  package_name, options1[i], options2[j]);
        }
        ++j;
      }
    }
    ++i;
  }
  
  return errors;
}

int
cmdline_parser_internal (
  int argc, char **argv, struct gengetopt_args_info *args_info,
                        struct cmdline_parser_params *params, const char *additional_error)
{
  int c;    /* Character of the parsed option.  */

  struct generic_list * resourceID_list = NULL;
  struct generic_list * resourceName_list = NULL;
  struct generic_list * resourceType_list = NULL;
  struct generic_list * resourceInstances_list = NULL;
  struct generic_list * resourceRequired_list = NULL;
  struct generic_list * resourceOperations_list = NULL;
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
        { "ipcAddress", 1, NULL, 'a' },
        { "ipcPort",    1, NULL, 'p' },
        { "xmlFile",    1, NULL, 'x' },
        { "objectID",   1, NULL, 'o' },
        { "objectName", 1, NULL, 'j' },
        { "objectMandatory",    0, NULL, 'm' },
        { "objectInstances",    1, NULL, 'y' },
        { "resourceID", 1, NULL, 'r' },
        { "resourceName",   1, NULL, 'n' },
        { "resourceType",   1, NULL, 't' },
        { "resourceInstances",  1, NULL, 'u' },
        { "resourceRequired",   1, NULL, 'q' },
        { "resourceOperations", 1, NULL, 'k' },
        { 0,  0, 0, 0 }
      };

      custom_optarg = optarg;
      custom_optind = optind;
      custom_opterr = opterr;
      custom_optopt = optopt;

      c = custom_getopt_long (argc, argv, "hVvda:p:x:o:j:my:r:n:t:u:q:k:", long_options, &option_index);

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
        case 'a':   /* Connect to Client IPC Address.  */
        
        
          if (update_arg( (void *)&(args_info->ipcAddress_arg), 
               &(args_info->ipcAddress_orig), &(args_info->ipcAddress_given),
              &(local_args_info.ipcAddress_given), optarg, 0, "127.0.0.1", ARG_STRING,
              check_ambiguity, override, 0, 0,
              "ipcAddress", 'a',
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
        case 'x':   /* Load XML Definition file.  */
          args_info->XML_Definition_mode_counter += 1;
        
        
          if (update_arg( (void *)&(args_info->xmlFile_arg), 
               &(args_info->xmlFile_orig), &(args_info->xmlFile_given),
              &(local_args_info.xmlFile_given), optarg, 0, 0, ARG_STRING,
              check_ambiguity, override, 0, 0,
              "xmlFile", 'x',
              additional_error))
            goto failure;
        
          break;
        case 'o':   /* Object ID.  */
          args_info->Cmdline_Definition_mode_counter += 1;
        
        
          if (update_arg( (void *)&(args_info->objectID_arg), 
               &(args_info->objectID_orig), &(args_info->objectID_given),
              &(local_args_info.objectID_given), optarg, 0, 0, ARG_INT,
              check_ambiguity, override, 0, 0,
              "objectID", 'o',
              additional_error))
            goto failure;
        
          break;
        case 'j':   /* Object name.  */
          args_info->Cmdline_Definition_mode_counter += 1;
        
        
          if (update_arg( (void *)&(args_info->objectName_arg), 
               &(args_info->objectName_orig), &(args_info->objectName_given),
              &(local_args_info.objectName_given), optarg, 0, 0, ARG_STRING,
              check_ambiguity, override, 0, 0,
              "objectName", 'j',
              additional_error))
            goto failure;
        
          break;
        case 'm':   /* Object is required or optional.  */
          args_info->Cmdline_Definition_mode_counter += 1;
        
        
          if (update_arg((void *)&(args_info->objectMandatory_flag), 0, &(args_info->objectMandatory_given),
              &(local_args_info.objectMandatory_given), optarg, 0, 0, ARG_FLAG,
              check_ambiguity, override, 1, 0, "objectMandatory", 'm',
              additional_error))
            goto failure;
        
          break;
        case 'y':   /* Object supports single or multiple instances.  */
          args_info->Cmdline_Definition_mode_counter += 1;
        
        
          if (update_arg( (void *)&(args_info->objectInstances_arg), 
               &(args_info->objectInstances_orig), &(args_info->objectInstances_given),
              &(local_args_info.objectInstances_given), optarg, cmdline_parser_objectInstances_values, "single", ARG_ENUM,
              check_ambiguity, override, 0, 0,
              "objectInstances", 'y',
              additional_error))
            goto failure;
        
          break;
        case 'r':   /* Resource ID.  */
          args_info->Cmdline_Definition_mode_counter += 1;
        
          if (update_multiple_arg_temp(&resourceID_list, 
              &(local_args_info.resourceID_given), optarg, 0, 0, ARG_INT,
              "resourceID", 'r',
              additional_error))
            goto failure;
        
          break;
        case 'n':   /* Resource Name.  */
          args_info->Cmdline_Definition_mode_counter += 1;
        
          if (update_multiple_arg_temp(&resourceName_list, 
              &(local_args_info.resourceName_given), optarg, 0, 0, ARG_STRING,
              "resourceName", 'n',
              additional_error))
            goto failure;
        
          break;
        case 't':   /* Resource Type.  */
          args_info->Cmdline_Definition_mode_counter += 1;
        
          if (update_multiple_arg_temp(&resourceType_list, 
              &(local_args_info.resourceType_given), optarg, cmdline_parser_resourceType_values, 0, ARG_ENUM,
              "resourceType", 't',
              additional_error))
            goto failure;
        
          break;
        case 'u':   /* Resource supports single or multiple instances.  */
          args_info->Cmdline_Definition_mode_counter += 1;
        
          if (update_multiple_arg_temp(&resourceInstances_list, 
              &(local_args_info.resourceInstances_given), optarg, cmdline_parser_resourceInstances_values, 0, ARG_ENUM,
              "resourceInstances", 'u',
              additional_error))
            goto failure;
        
          break;
        case 'q':   /* Resource is required or optional.  */
          args_info->Cmdline_Definition_mode_counter += 1;
        
          if (update_multiple_arg_temp(&resourceRequired_list, 
              &(local_args_info.resourceRequired_given), optarg, cmdline_parser_resourceRequired_values, 0, ARG_ENUM,
              "resourceRequired", 'q',
              additional_error))
            goto failure;
        
          break;
        case 'k':   /* Resource Operation.  */
          args_info->Cmdline_Definition_mode_counter += 1;
        
          if (update_multiple_arg_temp(&resourceOperations_list, 
              &(local_args_info.resourceOperations_given), optarg, cmdline_parser_resourceOperations_values, 0, ARG_ENUM,
              "resourceOperations", 'k',
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


  update_multiple_arg((void *)&(args_info->resourceID_arg),
    &(args_info->resourceID_orig), args_info->resourceID_given,
    local_args_info.resourceID_given, 0,
    ARG_INT, resourceID_list);
  update_multiple_arg((void *)&(args_info->resourceName_arg),
    &(args_info->resourceName_orig), args_info->resourceName_given,
    local_args_info.resourceName_given, 0,
    ARG_STRING, resourceName_list);
  update_multiple_arg((void *)&(args_info->resourceType_arg),
    &(args_info->resourceType_orig), args_info->resourceType_given,
    local_args_info.resourceType_given, 0,
    ARG_ENUM, resourceType_list);
  update_multiple_arg((void *)&(args_info->resourceInstances_arg),
    &(args_info->resourceInstances_orig), args_info->resourceInstances_given,
    local_args_info.resourceInstances_given, 0,
    ARG_ENUM, resourceInstances_list);
  update_multiple_arg((void *)&(args_info->resourceRequired_arg),
    &(args_info->resourceRequired_orig), args_info->resourceRequired_given,
    local_args_info.resourceRequired_given, 0,
    ARG_ENUM, resourceRequired_list);
  update_multiple_arg((void *)&(args_info->resourceOperations_arg),
    &(args_info->resourceOperations_orig), args_info->resourceOperations_given,
    local_args_info.resourceOperations_given, 0,
    ARG_ENUM, resourceOperations_list);

  args_info->resourceID_given += local_args_info.resourceID_given;
  local_args_info.resourceID_given = 0;
  args_info->resourceName_given += local_args_info.resourceName_given;
  local_args_info.resourceName_given = 0;
  args_info->resourceType_given += local_args_info.resourceType_given;
  local_args_info.resourceType_given = 0;
  args_info->resourceInstances_given += local_args_info.resourceInstances_given;
  local_args_info.resourceInstances_given = 0;
  args_info->resourceRequired_given += local_args_info.resourceRequired_given;
  local_args_info.resourceRequired_given = 0;
  args_info->resourceOperations_given += local_args_info.resourceOperations_given;
  local_args_info.resourceOperations_given = 0;
  
  if (args_info->Cmdline_Definition_mode_counter && args_info->XML_Definition_mode_counter) {
    int Cmdline_Definition_given[] = {args_info->objectID_given, args_info->objectName_given, args_info->objectMandatory_given, args_info->objectInstances_given, args_info->resourceID_given, args_info->resourceName_given, args_info->resourceType_given, args_info->resourceInstances_given, args_info->resourceRequired_given, args_info->resourceOperations_given,  -1};
    const char *Cmdline_Definition_desc[] = {"--objectID", "--objectName", "--objectMandatory", "--objectInstances", "--resourceID", "--resourceName", "--resourceType", "--resourceInstances", "--resourceRequired", "--resourceOperations",  0};
    int XML_Definition_given[] = {args_info->xmlFile_given,  -1};
    const char *XML_Definition_desc[] = {"--xmlFile",  0};
    error_occurred += check_modes(Cmdline_Definition_given, Cmdline_Definition_desc, XML_Definition_given, XML_Definition_desc);
  }
  
  if (check_required)
    {
      error_occurred += cmdline_parser_required2 (args_info, argv[0], additional_error);
    }

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
  free_list (resourceID_list, 0 );
  free_list (resourceName_list, 1 );
  free_list (resourceType_list, 0 );
  free_list (resourceInstances_list, 0 );
  free_list (resourceRequired_list, 0 );
  free_list (resourceOperations_list, 0 );
  
  cmdline_parser_release (&local_args_info);
  return (EXIT_FAILURE);
}
