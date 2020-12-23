/*   -*- buffer-read-only: t -*- vi: set ro:
 *
 *  DO NOT EDIT THIS FILE   (server-opts.c)
 *
 *  It has been AutoGen-ed
 *  From the definitions    server-opts.def
 *  and the template file   options
 *
 * Generated from AutoOpts 42:1:17 templates.
 *
 *  AutoOpts is a copyrighted work.  This source file is not encumbered
 *  by AutoOpts licensing, but is provided under the licensing terms chosen
 *  by the server author or copyright holder.  AutoOpts is
 *  licensed under the terms of the LGPL.  The redistributable library
 *  (``libopts'') is licensed under the terms of either the LGPL or, at the
 *  users discretion, the BSD license.  See the AutoOpts and/or libopts sources
 *  for details.
 */

#ifndef __doxygen__
#define OPTION_CODE_COMPILE 1
#include "server-opts.h"
#include <sys/types.h>
#include <sys/stat.h>

#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#ifdef  __cplusplus
extern "C" {
#endif
extern FILE * option_usage_fp;
#define zCopyright      NULL
#define zLicenseDescrip NULL

#ifndef NULL
#  define NULL 0
#endif

/**
 *  static const strings for server options
 */
static char const server_opt_strs[402] =
/*     0 */ "Address to bind\0"
/*    16 */ "ADDRESS\0"
/*    24 */ "address\0"
/*    32 */ "Port to bind\0"
/*    45 */ "PORT\0"
/*    50 */ "port\0"
/*    55 */ "Path to the log directory\0"
/*    81 */ "LOG_DIR\0"
/*    89 */ "log-dir\0"
/*    97 */ "Path to the maildir directory\0"
/*   127 */ "MAIL_DIR\0"
/*   136 */ "mail-dir\0"
/*   145 */ "Path to the client mails directory\0"
/*   180 */ "CLIENT_MAIL_DIR\0"
/*   196 */ "client-mail-dir\0"
/*   212 */ "display extended usage information and exit\0"
/*   256 */ "help\0"
/*   261 */ "extended usage information passed thru pager\0"
/*   306 */ "more-help\0"
/*   316 */ "SERVER\0"
/*   323 */ "server - SMTP server\n"
            "Usage:  %s { -<flag> [<val>] | --<name>[{=| }<val>] }...\n";

/**
 *  address option description:
 */
/** Descriptive text for the address option */
#define ADDRESS_DESC      (server_opt_strs+0)
/** Upper-cased name for the address option */
#define ADDRESS_NAME      (server_opt_strs+16)
/** Name string for the address option */
#define ADDRESS_name      (server_opt_strs+24)
/** Compiled in flag settings for the address option */
#define ADDRESS_FLAGS     (OPTST_DISABLED \
        | OPTST_SET_ARGTYPE(OPARG_TYPE_STRING))

/**
 *  port option description:
 */
/** Descriptive text for the port option */
#define PORT_DESC      (server_opt_strs+32)
/** Upper-cased name for the port option */
#define PORT_NAME      (server_opt_strs+45)
/** Name string for the port option */
#define PORT_name      (server_opt_strs+50)
/** Compiled in flag settings for the port option */
#define PORT_FLAGS     (OPTST_DISABLED \
        | OPTST_SET_ARGTYPE(OPARG_TYPE_NUMERIC))

/**
 *  log_dir option description:
 */
/** Descriptive text for the log_dir option */
#define LOG_DIR_DESC      (server_opt_strs+55)
/** Upper-cased name for the log_dir option */
#define LOG_DIR_NAME      (server_opt_strs+81)
/** Name string for the log_dir option */
#define LOG_DIR_name      (server_opt_strs+89)
/** Compiled in flag settings for the log_dir option */
#define LOG_DIR_FLAGS     (OPTST_DISABLED \
        | OPTST_SET_ARGTYPE(OPARG_TYPE_STRING))

/**
 *  mail_dir option description:
 */
/** Descriptive text for the mail_dir option */
#define MAIL_DIR_DESC      (server_opt_strs+97)
/** Upper-cased name for the mail_dir option */
#define MAIL_DIR_NAME      (server_opt_strs+127)
/** Name string for the mail_dir option */
#define MAIL_DIR_name      (server_opt_strs+136)
/** Compiled in flag settings for the mail_dir option */
#define MAIL_DIR_FLAGS     (OPTST_DISABLED \
        | OPTST_SET_ARGTYPE(OPARG_TYPE_STRING))

/**
 *  client_mail_dir option description:
 */
/** Descriptive text for the client_mail_dir option */
#define CLIENT_MAIL_DIR_DESC      (server_opt_strs+145)
/** Upper-cased name for the client_mail_dir option */
#define CLIENT_MAIL_DIR_NAME      (server_opt_strs+180)
/** Name string for the client_mail_dir option */
#define CLIENT_MAIL_DIR_name      (server_opt_strs+196)
/** Compiled in flag settings for the client_mail_dir option */
#define CLIENT_MAIL_DIR_FLAGS     (OPTST_DISABLED \
        | OPTST_SET_ARGTYPE(OPARG_TYPE_STRING))

/*
 *  Help/More_Help option descriptions:
 */
#define HELP_DESC       (server_opt_strs+212)
#define HELP_name       (server_opt_strs+256)
#ifdef HAVE_WORKING_FORK
#define MORE_HELP_DESC  (server_opt_strs+261)
#define MORE_HELP_name  (server_opt_strs+306)
#define MORE_HELP_FLAGS (OPTST_IMM | OPTST_NO_INIT)
#else
#define MORE_HELP_DESC  HELP_DESC
#define MORE_HELP_name  HELP_name
#define MORE_HELP_FLAGS (OPTST_OMITTED | OPTST_NO_INIT)
#endif
/**
 *  Declare option callback procedures
 */
extern tOptProc
    optionBooleanVal,   optionNestedVal,    optionNumericVal,
    optionPagedUsage,   optionPrintVersion, optionResetOpt,
    optionStackArg,     optionTimeDate,     optionTimeVal,
    optionUnstackArg,   optionVendorOption;
static tOptProc
    doOptPort, doUsageOpt;

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/**
 *  Define the server Option Descriptions.
 * This is an array of OPTION_CT entries, one for each
 * option that the server program responds to.
 */
static tOptDesc optDesc[OPTION_CT] = {
  {  /* entry idx, value */ 0, VALUE_OPT_ADDRESS,
     /* equiv idx, value */ 0, VALUE_OPT_ADDRESS,
     /* equivalenced to  */ NO_EQUIVALENT,
     /* min, max, act ct */ 1, 1, 0,
     /* opt state flags  */ ADDRESS_FLAGS, 0,
     /* last opt argumnt */ { NULL }, /* --address */
     /* arg list/cookie  */ NULL,
     /* must/cannot opts */ NULL, NULL,
     /* option proc      */ NULL,
     /* desc, NAME, name */ ADDRESS_DESC, ADDRESS_NAME, ADDRESS_name,
     /* disablement strs */ NULL, NULL },

  {  /* entry idx, value */ 1, VALUE_OPT_PORT,
     /* equiv idx, value */ 1, VALUE_OPT_PORT,
     /* equivalenced to  */ NO_EQUIVALENT,
     /* min, max, act ct */ 1, 1, 0,
     /* opt state flags  */ PORT_FLAGS, 0,
     /* last opt argumnt */ { NULL }, /* --port */
     /* arg list/cookie  */ NULL,
     /* must/cannot opts */ NULL, NULL,
     /* option proc      */ doOptPort,
     /* desc, NAME, name */ PORT_DESC, PORT_NAME, PORT_name,
     /* disablement strs */ NULL, NULL },

  {  /* entry idx, value */ 2, VALUE_OPT_LOG_DIR,
     /* equiv idx, value */ 2, VALUE_OPT_LOG_DIR,
     /* equivalenced to  */ NO_EQUIVALENT,
     /* min, max, act ct */ 0, 1, 0,
     /* opt state flags  */ LOG_DIR_FLAGS, 0,
     /* last opt argumnt */ { NULL }, /* --log_dir */
     /* arg list/cookie  */ NULL,
     /* must/cannot opts */ NULL, NULL,
     /* option proc      */ NULL,
     /* desc, NAME, name */ LOG_DIR_DESC, LOG_DIR_NAME, LOG_DIR_name,
     /* disablement strs */ NULL, NULL },

  {  /* entry idx, value */ 3, VALUE_OPT_MAIL_DIR,
     /* equiv idx, value */ 3, VALUE_OPT_MAIL_DIR,
     /* equivalenced to  */ NO_EQUIVALENT,
     /* min, max, act ct */ 0, 1, 0,
     /* opt state flags  */ MAIL_DIR_FLAGS, 0,
     /* last opt argumnt */ { NULL }, /* --mail_dir */
     /* arg list/cookie  */ NULL,
     /* must/cannot opts */ NULL, NULL,
     /* option proc      */ NULL,
     /* desc, NAME, name */ MAIL_DIR_DESC, MAIL_DIR_NAME, MAIL_DIR_name,
     /* disablement strs */ NULL, NULL },

  {  /* entry idx, value */ 4, VALUE_OPT_CLIENT_MAIL_DIR,
     /* equiv idx, value */ 4, VALUE_OPT_CLIENT_MAIL_DIR,
     /* equivalenced to  */ NO_EQUIVALENT,
     /* min, max, act ct */ 0, 1, 0,
     /* opt state flags  */ CLIENT_MAIL_DIR_FLAGS, 0,
     /* last opt argumnt */ { NULL }, /* --client_mail_dir */
     /* arg list/cookie  */ NULL,
     /* must/cannot opts */ NULL, NULL,
     /* option proc      */ NULL,
     /* desc, NAME, name */ CLIENT_MAIL_DIR_DESC, CLIENT_MAIL_DIR_NAME, CLIENT_MAIL_DIR_name,
     /* disablement strs */ NULL, NULL },

  {  /* entry idx, value */ INDEX_OPT_HELP, VALUE_OPT_HELP,
     /* equiv idx value  */ NO_EQUIVALENT, VALUE_OPT_HELP,
     /* equivalenced to  */ NO_EQUIVALENT,
     /* min, max, act ct */ 0, 1, 0,
     /* opt state flags  */ OPTST_IMM | OPTST_NO_INIT, AOUSE_HELP,
     /* last opt argumnt */ { NULL },
     /* arg list/cookie  */ NULL,
     /* must/cannot opts */ NULL, NULL,
     /* option proc      */ doUsageOpt,
     /* desc, NAME, name */ HELP_DESC, NULL, HELP_name,
     /* disablement strs */ NULL, NULL },

  {  /* entry idx, value */ INDEX_OPT_MORE_HELP, VALUE_OPT_MORE_HELP,
     /* equiv idx value  */ NO_EQUIVALENT, VALUE_OPT_MORE_HELP,
     /* equivalenced to  */ NO_EQUIVALENT,
     /* min, max, act ct */ 0, 1, 0,
     /* opt state flags  */ MORE_HELP_FLAGS, AOUSE_MORE_HELP,
     /* last opt argumnt */ { NULL },
     /* arg list/cookie  */ NULL,
     /* must/cannot opts */ NULL,  NULL,
     /* option proc      */ optionPagedUsage,
     /* desc, NAME, name */ MORE_HELP_DESC, NULL, MORE_HELP_name,
     /* disablement strs */ NULL, NULL }
};


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/** Reference to the upper cased version of server. */
#define zPROGNAME       (server_opt_strs+316)
/** Reference to the title line for server usage. */
#define zUsageTitle     (server_opt_strs+323)
/** There is no server configuration file. */
#define zRcName         NULL
/** There are no directories to search for server config files. */
#define apzHomeList     NULL
/** The server program bug email address. */
#define zBugsAddr       (NULL)
/** Clarification/explanation of what server does. */
#define zExplain        (NULL)
/** Extra detail explaining what server does. */
#define zDetail         (NULL)
/** The full version string for server. */
#define zFullVersion    (NULL)
/* extracted from optcode.tlib near line 342 */

#if defined(ENABLE_NLS)
# define OPTPROC_BASE OPTPROC_TRANSLATE
  static tOptionXlateProc translate_option_strings;
#else
# define OPTPROC_BASE OPTPROC_NONE
# define translate_option_strings NULL
#endif /* ENABLE_NLS */

#define server_full_usage (NULL)
#define server_short_usage (NULL)

#endif /* not defined __doxygen__ */

/*
 *  Create the static procedure(s) declared above.
 */
/**
 * The callout function that invokes the optionUsage function.
 *
 * @param[in] opts the AutoOpts option description structure
 * @param[in] od   the descriptor for the "help" (usage) option.
 * @noreturn
 */
static void
doUsageOpt(tOptions * opts, tOptDesc * od)
{
    int ex_code;
    ex_code = SERVER_EXIT_SUCCESS;
    optionUsage(&serverOptions, ex_code);
    /* NOTREACHED */
    exit(SERVER_EXIT_FAILURE);
    (void)opts;
    (void)od;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/**
 * Code to handle the port option.
 *
 * @param[in] pOptions the server options data structure
 * @param[in,out] pOptDesc the option descriptor for this option.
 */
static void
doOptPort(tOptions* pOptions, tOptDesc* pOptDesc)
{
    static struct {long rmin, rmax;} const rng[2] = {
        { 110, LONG_MIN },  { 1024, 65000 } };
    int  ix;

    if (pOptions <= OPTPROC_EMIT_LIMIT)
        goto emit_ranges;
    optionNumericVal(pOptions, pOptDesc);

    for (ix = 0; ix < 2; ix++) {
        if (pOptDesc->optArg.argInt < rng[ix].rmin)
            continue;  /* ranges need not be ordered. */
        if (pOptDesc->optArg.argInt == rng[ix].rmin)
            return;
        if (rng[ix].rmax == LONG_MIN)
            continue;
        if (pOptDesc->optArg.argInt <= rng[ix].rmax)
            return;
    }

    option_usage_fp = stderr;

 emit_ranges:
optionShowRange(pOptions, pOptDesc, VOIDP(rng), 2);
}
/* extracted from optmain.tlib near line 1250 */

/**
 * The directory containing the data associated with server.
 */
#ifndef  PKGDATADIR
# define PKGDATADIR ""
#endif

/**
 * Information about the person or institution that packaged server
 * for the current distribution.
 */
#ifndef  WITH_PACKAGER
# define server_packager_info NULL
#else
/** Packager information for server. */
static char const server_packager_info[] =
    "Packaged by " WITH_PACKAGER

# ifdef WITH_PACKAGER_VERSION
        " ("WITH_PACKAGER_VERSION")"
# endif

# ifdef WITH_PACKAGER_BUG_REPORTS
    "\nReport server bugs to " WITH_PACKAGER_BUG_REPORTS
# endif
    "\n";
#endif
#ifndef __doxygen__

#endif /* __doxygen__ */
/**
 * The option definitions for server.  The one structure that
 * binds them all.
 */
tOptions serverOptions = {
    OPTIONS_STRUCT_VERSION,
    0, NULL,                    /* original argc + argv    */
    ( OPTPROC_BASE
    + OPTPROC_ERRSTOP
    + OPTPROC_SHORTOPT
    + OPTPROC_LONGOPT
    + OPTPROC_NO_ARGS
    + OPTPROC_GNUUSAGE ),
    0, NULL,                    /* current option index, current option */
    NULL,         NULL,         zPROGNAME,
    zRcName,      zCopyright,   zLicenseDescrip,
    zFullVersion, apzHomeList,  zUsageTitle,
    zExplain,     zDetail,      optDesc,
    zBugsAddr,                  /* address to send bugs to */
    NULL, NULL,                 /* extensions/saved state  */
    optionUsage, /* usage procedure */
    translate_option_strings,   /* translation procedure */
    /*
     *  Indexes to special options
     */
    { INDEX_OPT_MORE_HELP, /* more-help option index */
      NO_EQUIVALENT, /* save option index */
      NO_EQUIVALENT, /* '-#' option index */
      NO_EQUIVALENT /* index of default opt */
    },
    7 /* full option count */, 5 /* user option count */,
    server_full_usage, server_short_usage,
    NULL, NULL,
    PKGDATADIR, server_packager_info
};

#if ENABLE_NLS
/**
 * This code is designed to translate translatable option text for the
 * server program.  These translations happen upon entry
 * to optionProcess().
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#ifdef HAVE_DCGETTEXT
# include <gettext.h>
#endif
#include <autoopts/usage-txt.h>

static char * AO_gettext(char const * pz);
static void   coerce_it(void ** s);

/**
 * AutoGen specific wrapper function for gettext.  It relies on the macro _()
 * to convert from English to the target language, then strdup-duplicates the
 * result string.  It tries the "libopts" domain first, then whatever has been
 * set via the \a textdomain(3) call.
 *
 * @param[in] pz the input text used as a lookup key.
 * @returns the translated text (if there is one),
 *   or the original text (if not).
 */
static char *
AO_gettext(char const * pz)
{
    char * res;
    if (pz == NULL)
        return NULL;
#ifdef HAVE_DCGETTEXT
    /*
     * While processing the option_xlateable_txt data, try to use the
     * "libopts" domain.  Once we switch to the option descriptor data,
     * do *not* use that domain.
     */
    if (option_xlateable_txt.field_ct != 0) {
        res = dgettext("libopts", pz);
        if (res == pz)
            res = (char *)VOIDP(_(pz));
    } else
        res = (char *)VOIDP(_(pz));
#else
    res = (char *)VOIDP(_(pz));
#endif
    if (res == pz)
        return res;
    res = strdup(res);
    if (res == NULL) {
        fputs(_("No memory for duping translated strings\n"), stderr);
        exit(SERVER_EXIT_FAILURE);
    }
    return res;
}

/**
 * All the pointers we use are marked "* const", but they are stored in
 * writable memory.  Coerce the mutability and set the pointer.
 */
static void coerce_it(void ** s) { *s = AO_gettext(*s);
}

/**
 * Translate all the translatable strings in the serverOptions
 * structure defined above.  This is done only once.
 */
static void
translate_option_strings(void)
{
    tOptions * const opts = &serverOptions;

    /*
     *  Guard against re-translation.  It won't work.  The strings will have
     *  been changed by the first pass through this code.  One shot only.
     */
    if (option_xlateable_txt.field_ct != 0) {
        /*
         *  Do the translations.  The first pointer follows the field count
         *  field.  The field count field is the size of a pointer.
         */
        char ** ppz = (char**)VOIDP(&(option_xlateable_txt));
        int     ix  = option_xlateable_txt.field_ct;

        do {
            ppz++; /* skip over field_ct */
            *ppz = AO_gettext(*ppz);
        } while (--ix > 0);
        /* prevent re-translation and disable "libopts" domain lookup */
        option_xlateable_txt.field_ct = 0;

        coerce_it(VOIDP(&(opts->pzCopyright)));
        coerce_it(VOIDP(&(opts->pzCopyNotice)));
        coerce_it(VOIDP(&(opts->pzFullVersion)));
        coerce_it(VOIDP(&(opts->pzUsageTitle)));
        coerce_it(VOIDP(&(opts->pzExplain)));
        coerce_it(VOIDP(&(opts->pzDetail)));
        {
            tOptDesc * od = opts->pOptDesc;
            for (ix = opts->optCt; ix > 0; ix--, od++)
                coerce_it(VOIDP(&(od->pzText)));
        }
    }
}
#endif /* ENABLE_NLS */

#ifdef DO_NOT_COMPILE_THIS_CODE_IT_IS_FOR_GETTEXT
/** I18N function strictly for xgettext.  Do not compile. */
static void bogus_function(void) {
  /* TRANSLATORS:

     The following dummy function was crated solely so that xgettext can
     extract the correct strings.  These strings are actually referenced
     by a field name in the serverOptions structure noted in the
     comments below.  The literal text is defined in server_opt_strs.

     NOTE: the strings below are segmented with respect to the source string
     server_opt_strs.  The strings above are handed off for translation
     at run time a paragraph at a time.  Consequently, they are presented here
     for translation a paragraph at a time.

     ALSO: often the description for an option will reference another option
     by name.  These are set off with apostrophe quotes (I hope).  Do not
     translate option names.
   */
  /* referenced via serverOptions.pOptDesc->pzText */
  puts(_("Address to bind"));

  /* referenced via serverOptions.pOptDesc->pzText */
  puts(_("Port to bind"));

  /* referenced via serverOptions.pOptDesc->pzText */
  puts(_("Path to the log directory"));

  /* referenced via serverOptions.pOptDesc->pzText */
  puts(_("Path to the maildir directory"));

  /* referenced via serverOptions.pOptDesc->pzText */
  puts(_("Path to the client mails directory"));

  /* referenced via serverOptions.pOptDesc->pzText */
  puts(_("display extended usage information and exit"));

  /* referenced via serverOptions.pOptDesc->pzText */
  puts(_("extended usage information passed thru pager"));

  /* referenced via serverOptions.pzUsageTitle */
  puts(_("server - SMTP server\n\
Usage:  %s { -<flag> [<val>] | --<name>[{=| }<val>] }...\n"));

  /* referenced via serverOptions.pzFullUsage */
  puts(_("<<<NOT-FOUND>>>"));

  /* referenced via serverOptions.pzShortUsage */
  puts(_("<<<NOT-FOUND>>>"));
  /* LIBOPTS-MESSAGES: */
#line 67 "../autoopts.c"
  puts(_("allocation of %d bytes failed\n"));
#line 89 "../autoopts.c"
  puts(_("allocation of %d bytes failed\n"));
#line 48 "../init.c"
  puts(_("AutoOpts function called without option descriptor\n"));
#line 81 "../init.c"
  puts(_("\tThis exceeds the compiled library version:  "));
#line 79 "../init.c"
  puts(_("Automated Options Processing Error!\n"
       "\t%s called AutoOpts function with structure version %d:%d:%d.\n"));
#line 78 "../autoopts.c"
  puts(_("realloc of %d bytes at 0x%p failed\n"));
#line 83 "../init.c"
  puts(_("\tThis is less than the minimum library version:  "));
#line 121 "../version.c"
  puts(_("Automated Options version %s\n"
       "\tCopyright (C) 1999-2017 by Bruce Korb - all rights reserved\n"));
#line 49 "../makeshell.c"
  puts(_("(AutoOpts bug):  %s.\n"));
#line 90 "../reset.c"
  puts(_("optionResetOpt() called, but reset-option not configured"));
#line 241 "../usage.c"
  puts(_("could not locate the 'help' option"));
#line 330 "../autoopts.c"
  puts(_("optionProcess() was called with invalid data"));
#line 697 "../usage.c"
  puts(_("invalid argument type specified"));
#line 568 "../find.c"
  puts(_("defaulted to option with optional arg"));
#line 76 "../alias.c"
  puts(_("aliasing option is out of range."));
#line 210 "../enum.c"
  puts(_("%s error:  the keyword '%s' is ambiguous for %s\n"));
#line 78 "../find.c"
  puts(_("  The following options match:\n"));
#line 263 "../find.c"
  puts(_("%s: ambiguous option name: %s (matches %d options)\n"));
#line 161 "../check.c"
  puts(_("%s: Command line arguments required\n"));
#line 43 "../alias.c"
  puts(_("%d %s%s options allowed\n"));
#line 56 "../makeshell.c"
  puts(_("%s error %d (%s) calling %s for '%s'\n"));
#line 268 "../makeshell.c"
  puts(_("interprocess pipe"));
#line 171 "../version.c"
  puts(_("error: version option argument '%c' invalid.  Use:\n"
       "\t'v' - version only\n"
       "\t'c' - version and copyright\n"
       "\t'n' - version and full copyright notice\n"));
#line 58 "../check.c"
  puts(_("%s error:  the '%s' and '%s' options conflict\n"));
#line 187 "../find.c"
  puts(_("%s: The '%s' option has been disabled."));
#line 400 "../find.c"
  puts(_("%s: The '%s' option has been disabled."));
#line 38 "../alias.c"
  puts(_("-equivalence"));
#line 439 "../find.c"
  puts(_("%s: illegal option -- %c\n"));
#line 110 "../reset.c"
  puts(_("%s: illegal option -- %c\n"));
#line 241 "../find.c"
  puts(_("%s: illegal option -- %s\n"));
#line 740 "../find.c"
  puts(_("%s: illegal option -- %s\n"));
#line 118 "../reset.c"
  puts(_("%s: illegal option -- %s\n"));
#line 305 "../find.c"
  puts(_("%s: unknown vendor extension option -- %s\n"));
#line 135 "../enum.c"
  puts(_("  or an integer from %d through %d\n"));
#line 145 "../enum.c"
  puts(_("  or an integer from %d through %d\n"));
#line 696 "../usage.c"
  puts(_("%s error:  invalid option descriptor for %s\n"));
#line 1030 "../usage.c"
  puts(_("%s error:  invalid option descriptor for %s\n"));
#line 355 "../find.c"
  puts(_("%s: invalid option name: %s\n"));
#line 497 "../find.c"
  puts(_("%s: The '%s' option requires an argument.\n"));
#line 150 "../autoopts.c"
  puts(_("(AutoOpts bug):  Equivalenced option '%s' was equivalenced to both\n"
       "\t'%s' and '%s'."));
#line 94 "../check.c"
  puts(_("%s error:  The %s option is required\n"));
#line 602 "../find.c"
  puts(_("%s: The '%s' option cannot have an argument.\n"));
#line 151 "../check.c"
  puts(_("%s: Command line arguments are not allowed.\n"));
#line 568 "../save.c"
  puts(_("error %d (%s) creating %s\n"));
#line 210 "../enum.c"
  puts(_("%s error:  '%s' does not match any %s keywords.\n"));
#line 93 "../reset.c"
  puts(_("%s error: The '%s' option requires an argument.\n"));
#line 122 "../save.c"
  puts(_("error %d (%s) stat-ing %s\n"));
#line 175 "../save.c"
  puts(_("error %d (%s) stat-ing %s\n"));
#line 143 "../restore.c"
  puts(_("%s error: no saved option state\n"));
#line 225 "../autoopts.c"
  puts(_("'%s' is not a command line option.\n"));
#line 113 "../time.c"
  puts(_("%s error:  '%s' is not a recognizable date/time.\n"));
#line 50 "../time.c"
  puts(_("%s error:  '%s' is not a recognizable time duration.\n"));
#line 92 "../check.c"
  puts(_("%s error:  The %s option must appear %d times.\n"));
#line 165 "../numeric.c"
  puts(_("%s error:  '%s' is not a recognizable number.\n"));
#line 176 "../enum.c"
  puts(_("%s error:  %s exceeds %s keyword count\n"));
#line 279 "../usage.c"
  puts(_("Try '%s %s' for more information.\n"));
#line 45 "../alias.c"
  puts(_("one %s%s option allowed\n"));
#line 170 "../makeshell.c"
  puts(_("standard output"));
#line 905 "../makeshell.c"
  puts(_("standard output"));
#line 223 "../usage.c"
  puts(_("standard output"));
#line 364 "../usage.c"
  puts(_("standard output"));
#line 574 "../usage.c"
  puts(_("standard output"));
#line 178 "../version.c"
  puts(_("standard output"));
#line 223 "../usage.c"
  puts(_("standard error"));
#line 364 "../usage.c"
  puts(_("standard error"));
#line 574 "../usage.c"
  puts(_("standard error"));
#line 178 "../version.c"
  puts(_("standard error"));
#line 170 "../makeshell.c"
  puts(_("write"));
#line 905 "../makeshell.c"
  puts(_("write"));
#line 222 "../usage.c"
  puts(_("write"));
#line 363 "../usage.c"
  puts(_("write"));
#line 573 "../usage.c"
  puts(_("write"));
#line 177 "../version.c"
  puts(_("write"));
#line 60 "../numeric.c"
  puts(_("%s error:  %s option value %ld is out of range.\n"));
#line 44 "../check.c"
  puts(_("%s error:  %s option requires the %s option\n"));
#line 121 "../save.c"
  puts(_("%s warning:  cannot save options - %s not regular file\n"));
#line 174 "../save.c"
  puts(_("%s warning:  cannot save options - %s not regular file\n"));
#line 193 "../save.c"
  puts(_("%s warning:  cannot save options - %s not regular file\n"));
#line 567 "../save.c"
  puts(_("%s warning:  cannot save options - %s not regular file\n"));
  /* END-LIBOPTS-MESSAGES */

  /* USAGE-TEXT: */
#line 822 "../usage.c"
  puts(_("\t\t\t\t- an alternate for '%s'\n"));
#line 1097 "../usage.c"
  puts(_("Version, usage and configuration options:"));
#line 873 "../usage.c"
  puts(_("\t\t\t\t- default option for unnamed options\n"));
#line 786 "../usage.c"
  puts(_("\t\t\t\t- disabled as '--%s'\n"));
#line 1066 "../usage.c"
  puts(_(" --- %-14s %s\n"));
#line 1064 "../usage.c"
  puts(_("This option has been disabled"));
#line 813 "../usage.c"
  puts(_("\t\t\t\t- enabled by default\n"));
#line 40 "../alias.c"
  puts(_("%s error:  only "));
#line 1143 "../usage.c"
  puts(_(" - examining environment variables named %s_*\n"));
#line 168 "../file.c"
  puts(_("\t\t\t\t- file must not pre-exist\n"));
#line 172 "../file.c"
  puts(_("\t\t\t\t- file must pre-exist\n"));
#line 329 "../usage.c"
  puts(_("Options are specified by doubled hyphens and their name or by a single\n"
       "hyphen and the flag character.\n"));
#line 882 "../makeshell.c"
  puts(_("\n"
       "= = = = = = = =\n\n"
       "This incarnation of genshell will produce\n"
       "a shell script to parse the options for %s:\n\n"));
#line 142 "../enum.c"
  puts(_("  or an integer mask with any of the lower %d bits set\n"));
#line 846 "../usage.c"
  puts(_("\t\t\t\t- is a set membership option\n"));
#line 867 "../usage.c"
  puts(_("\t\t\t\t- must appear between %d and %d times\n"));
#line 331 "../usage.c"
  puts(_("Options are specified by single or double hyphens and their name.\n"));
#line 853 "../usage.c"
  puts(_("\t\t\t\t- may appear multiple times\n"));
#line 840 "../usage.c"
  puts(_("\t\t\t\t- may not be preset\n"));
#line 1258 "../usage.c"
  puts(_("   Arg Option-Name    Description\n"));
#line 1194 "../usage.c"
  puts(_("  Flg Arg Option-Name    Description\n"));
#line 1252 "../usage.c"
  puts(_("  Flg Arg Option-Name    Description\n"));
#line 1253 "../usage.c"
  puts(_(" %3s %s"));
#line 1259 "../usage.c"
  puts(_(" %3s %s"));
#line 336 "../usage.c"
  puts(_("The '-#<number>' option may omit the hash char\n"));
#line 332 "../usage.c"
  puts(_("All arguments are named options.\n"));
#line 920 "../usage.c"
  puts(_(" - reading file %s"));
#line 358 "../usage.c"
  puts(_("\n"
       "Please send bug reports to:  <%s>\n"));
#line 100 "../version.c"
  puts(_("\n"
       "Please send bug reports to:  <%s>\n"));
#line 129 "../version.c"
  puts(_("\n"
       "Please send bug reports to:  <%s>\n"));
#line 852 "../usage.c"
  puts(_("\t\t\t\t- may NOT appear - preset only\n"));
#line 893 "../usage.c"
  puts(_("\n"
       "The following option preset mechanisms are supported:\n"));
#line 1141 "../usage.c"
  puts(_("\n"
       "The following option preset mechanisms are supported:\n"));
#line 631 "../usage.c"
  puts(_("prohibits these options:\n"));
#line 626 "../usage.c"
  puts(_("prohibits the option '%s'\n"));
#line 81 "../numeric.c"
  puts(_("%s%ld to %ld"));
#line 79 "../numeric.c"
  puts(_("%sgreater than or equal to %ld"));
#line 75 "../numeric.c"
  puts(_("%s%ld exactly"));
#line 68 "../numeric.c"
  puts(_("%sit must lie in one of the ranges:\n"));
#line 68 "../numeric.c"
  puts(_("%sit must be in the range:\n"));
#line 88 "../numeric.c"
  puts(_(", or\n"));
#line 66 "../numeric.c"
  puts(_("%sis scalable with a suffix: k/K/m/M/g/G/t/T\n"));
#line 77 "../numeric.c"
  puts(_("%sless than or equal to %ld"));
#line 339 "../usage.c"
  puts(_("Operands and options may be intermixed.  They will be reordered.\n"));
#line 601 "../usage.c"
  puts(_("requires the option '%s'\n"));
#line 604 "../usage.c"
  puts(_("requires these options:\n"));
#line 1270 "../usage.c"
  puts(_("   Arg Option-Name   Req?  Description\n"));
#line 1264 "../usage.c"
  puts(_("  Flg Arg Option-Name   Req?  Description\n"));
#line 143 "../enum.c"
  puts(_("or you may use a numeric representation.  Preceding these with a '!'\n"
       "will clear the bits, specifying 'none' will clear all bits, and 'all'\n"
       "will set them all.  Multiple entries may be passed as an option\n"
       "argument list.\n"));
#line 859 "../usage.c"
  puts(_("\t\t\t\t- may appear up to %d times\n"));
#line 52 "../enum.c"
  puts(_("The valid \"%s\" option keywords are:\n"));
#line 1101 "../usage.c"
  puts(_("The next option supports vendor supported extra options:"));
#line 722 "../usage.c"
  puts(_("These additional options are:"));
  /* END-USAGE-TEXT */
}
#endif /* uncompilable code */
#ifdef  __cplusplus
}
#endif
/* server-opts.c ends here */
