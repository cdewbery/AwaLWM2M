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


#ifndef MAIN_CMDLINE_H
#define MAIN_CMDLINE_H

/* If we use autoconf.  */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h> /* for FILE */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifndef CMDLINE_PARSER_PACKAGE
/** @brief the program name (used for printing errors) */
#define CMDLINE_PARSER_PACKAGE "test_runner"
#endif

#ifndef CMDLINE_PARSER_PACKAGE_NAME
/** @brief the complete program name (used for help and version) */
#define CMDLINE_PARSER_PACKAGE_NAME "test_runner"
#endif

#ifndef CMDLINE_PARSER_VERSION
/** @brief the program version */
#define CMDLINE_PARSER_VERSION "0.1"
#endif

/** @brief Where the command line options are stored */
struct gengetopt_args_info
{
  const char *help_help; /**< @brief Print help and exit help description.  */
  const char *version_help; /**< @brief Print version and exit help description.  */
  int clientIpcPort_arg;    /**< @brief External LWM2M Client IPC Port.  */
  char * clientIpcPort_orig;    /**< @brief External LWM2M Client IPC Port original value given at command line.  */
  const char *clientIpcPort_help; /**< @brief External LWM2M Client IPC Port help description.  */
  int serverIpcPort_arg;    /**< @brief External LWM2M Server IPC Port.  */
  char * serverIpcPort_orig;    /**< @brief External LWM2M Server IPC Port original value given at command line.  */
  const char *serverIpcPort_help; /**< @brief External LWM2M Server IPC Port help description.  */
  int clientLocalCoapPort_arg;  /**< @brief External LWM2M Client local CoAP Port.  */
  char * clientLocalCoapPort_orig;  /**< @brief External LWM2M Client local CoAP Port original value given at command line.  */
  const char *clientLocalCoapPort_help; /**< @brief External LWM2M Client local CoAP Port help description.  */
  int serverCoapPort_arg;   /**< @brief External LWM2M Server CoAP Port.  */
  char * serverCoapPort_orig;   /**< @brief External LWM2M Server CoAP Port original value given at command line.  */
  const char *serverCoapPort_help; /**< @brief External LWM2M Server CoAP Port help description.  */
  int logLevel_arg; /**< @brief Awa API Log Level.  */
  char * logLevel_orig; /**< @brief Awa API Log Level original value given at command line.  */
  const char *logLevel_help; /**< @brief Awa API Log Level help description.  */
  char * coapClientPath_arg;    /**< @brief Path to libcoap coap-client tool (default='lib/libcoap/examples/coap-client').  */
  char * coapClientPath_orig;   /**< @brief Path to libcoap coap-client tool original value given at command line.  */
  const char *coapClientPath_help; /**< @brief Path to libcoap coap-client tool help description.  */
  char * clientDaemonPath_arg;  /**< @brief Path to LWM2M client daemon (default='core/src/client/awa_clientd').  */
  char * clientDaemonPath_orig; /**< @brief Path to LWM2M client daemon original value given at command line.  */
  const char *clientDaemonPath_help; /**< @brief Path to LWM2M client daemon help description.  */
  char * serverDaemonPath_arg;  /**< @brief Path to LWM2M server daemon (default='core/src/server/awa_serverd').  */
  char * serverDaemonPath_orig; /**< @brief Path to LWM2M server daemon original value given at command line.  */
  const char *serverDaemonPath_help; /**< @brief Path to LWM2M server daemon help description.  */
  char * bootstrapDaemonPath_arg;   /**< @brief Path to LWM2M bootstrap daemon (default='core/src/bootstrap/awa_bootstrapd').  */
  char * bootstrapDaemonPath_orig;  /**< @brief Path to LWM2M bootstrap daemon original value given at command line.  */
  const char *bootstrapDaemonPath_help; /**< @brief Path to LWM2M bootstrap daemon help description.  */
  char * bootstrapConfig_arg;   /**< @brief Path to bootstrap config file (default='../api/tests/bootstrap-gtest.config').  */
  char * bootstrapConfig_orig;  /**< @brief Path to bootstrap config file original value given at command line.  */
  const char *bootstrapConfig_help; /**< @brief Path to bootstrap config file help description.  */
  
  unsigned int help_given ; /**< @brief Whether help was given.  */
  unsigned int version_given ;  /**< @brief Whether version was given.  */
  unsigned int clientIpcPort_given ;    /**< @brief Whether clientIpcPort was given.  */
  unsigned int serverIpcPort_given ;    /**< @brief Whether serverIpcPort was given.  */
  unsigned int clientLocalCoapPort_given ;  /**< @brief Whether clientLocalCoapPort was given.  */
  unsigned int serverCoapPort_given ;   /**< @brief Whether serverCoapPort was given.  */
  unsigned int logLevel_given ; /**< @brief Whether logLevel was given.  */
  unsigned int coapClientPath_given ;   /**< @brief Whether coapClientPath was given.  */
  unsigned int clientDaemonPath_given ; /**< @brief Whether clientDaemonPath was given.  */
  unsigned int serverDaemonPath_given ; /**< @brief Whether serverDaemonPath was given.  */
  unsigned int bootstrapDaemonPath_given ;  /**< @brief Whether bootstrapDaemonPath was given.  */
  unsigned int bootstrapConfig_given ;  /**< @brief Whether bootstrapConfig was given.  */

} ;

/** @brief The additional parameters to pass to parser functions */
struct cmdline_parser_params
{
  int override; /**< @brief whether to override possibly already present options (default 0) */
  int initialize; /**< @brief whether to initialize the option structure gengetopt_args_info (default 1) */
  int check_required; /**< @brief whether to check that all required options were provided (default 1) */
  int check_ambiguity; /**< @brief whether to check for options already specified in the option structure gengetopt_args_info (default 0) */
  int print_errors; /**< @brief whether getopt_long should print an error message for a bad option (default 1) */
} ;

/** @brief the purpose string of the program */
extern const char *gengetopt_args_info_purpose;
/** @brief the usage string of the program */
extern const char *gengetopt_args_info_usage;
/** @brief the description string of the program */
extern const char *gengetopt_args_info_description;
/** @brief all the lines making the help output */
extern const char *gengetopt_args_info_help[];

/**
 * The command line parser
 * @param argc the number of command line options
 * @param argv the command line options
 * @param args_info the structure where option information will be stored
 * @return 0 if everything went fine, NON 0 if an error took place
 */
int cmdline_parser (int argc, char **argv,
  struct gengetopt_args_info *args_info);

/**
 * The command line parser (version with additional parameters - deprecated)
 * @param argc the number of command line options
 * @param argv the command line options
 * @param args_info the structure where option information will be stored
 * @param override whether to override possibly already present options
 * @param initialize whether to initialize the option structure my_args_info
 * @param check_required whether to check that all required options were provided
 * @return 0 if everything went fine, NON 0 if an error took place
 * @deprecated use cmdline_parser_ext() instead
 */
int cmdline_parser2 (int argc, char **argv,
  struct gengetopt_args_info *args_info,
  int override, int initialize, int check_required);

/**
 * The command line parser (version with additional parameters)
 * @param argc the number of command line options
 * @param argv the command line options
 * @param args_info the structure where option information will be stored
 * @param params additional parameters for the parser
 * @return 0 if everything went fine, NON 0 if an error took place
 */
int cmdline_parser_ext (int argc, char **argv,
  struct gengetopt_args_info *args_info,
  struct cmdline_parser_params *params);

/**
 * Save the contents of the option struct into an already open FILE stream.
 * @param outfile the stream where to dump options
 * @param args_info the option struct to dump
 * @return 0 if everything went fine, NON 0 if an error took place
 */
int cmdline_parser_dump(FILE *outfile,
  struct gengetopt_args_info *args_info);

/**
 * Save the contents of the option struct into a (text) file.
 * This file can be read by the config file parser (if generated by gengetopt)
 * @param filename the file where to save
 * @param args_info the option struct to save
 * @return 0 if everything went fine, NON 0 if an error took place
 */
int cmdline_parser_file_save(const char *filename,
  struct gengetopt_args_info *args_info);

/**
 * Print the help
 */
void cmdline_parser_print_help(void);
/**
 * Print the version
 */
void cmdline_parser_print_version(void);

/**
 * Initializes all the fields a cmdline_parser_params structure 
 * to their default values
 * @param params the structure to initialize
 */
void cmdline_parser_params_init(struct cmdline_parser_params *params);

/**
 * Allocates dynamically a cmdline_parser_params structure and initializes
 * all its fields to their default values
 * @return the created and initialized cmdline_parser_params structure
 */
struct cmdline_parser_params *cmdline_parser_params_create(void);

/**
 * Initializes the passed gengetopt_args_info structure's fields
 * (also set default values for options that have a default)
 * @param args_info the structure to initialize
 */
void cmdline_parser_init (struct gengetopt_args_info *args_info);
/**
 * Deallocates the string fields of the gengetopt_args_info structure
 * (but does not deallocate the structure itself)
 * @param args_info the structure to deallocate
 */
void cmdline_parser_free (struct gengetopt_args_info *args_info);

/**
 * Checks that all the required options were specified
 * @param args_info the structure to check
 * @param prog_name the name of the program that will be used to print
 *   possible errors
 * @return
 */
int cmdline_parser_required (struct gengetopt_args_info *args_info,
  const char *prog_name);


#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* MAIN_CMDLINE_H */
