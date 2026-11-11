// about.cpp
// The program. Name, version, paths, and flags live here.
// Logger binds this object. Invocation, AboutCli, and Help bind Logger.
// g_print and g_printerr in this file report a rejected command.
// They do not write a log line.

#include "about.h"
#include "aboutcli.h"
#include "help.h"
#include "invocation.h"
#include "logger.h"
#include "project.h"

#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Purpose:
//   Load the compile-time identity and read JSON, QUIET, and DEBUG.
// Parameters:
//   None.
// Dependencies:
//   project.h macros, the process environment.
// Why This Design:
//   Later classes read this object. They do not read the macros again.
// CIAO Principles Applied:
//   Intentional: one load of the name, the version, and the flags.
// Protection Rule:
//   project.h stays the macro file. This object is the runtime source.
// Last updated: 2026-11-11
About::About(void)
    : versionMajorValue(VERSION_MAJOR),
      versionMinorValue(VERSION_MINOR),
      versionPatchValue(VERSION_PATCH),
      logDirValue(NULL),
      baseDirValue(NULL),
      quietMode(false),
      jsonMode(false),
      debugEnabled(false),
      verbose(0),
      purpose(0),
      prefix(NULL),
      home(NULL),
      datestamp(NULL),
      sawQuiet(0),
      sawJson(0),
      sawDebug(0),
      sawVerbose(0),
      page(0),
      reader(0)
{
    baseStorage[0] = '\0';
    if (g_snprintf(nameStorage, sizeof nameStorage, "%s", PROJECT_NAME) >= (gint)sizeof nameStorage) {
        g_printerr("logger name is too long\n");
        exit(1);
    }
    if (g_snprintf(descriptionStorage, sizeof descriptionStorage, "%s", PROJECT_DESCRIPTION) >= (gint)sizeof descriptionStorage) {
        g_printerr("logger name is too long\n");
        exit(1);
    }
    if (g_snprintf(componentStorage, sizeof componentStorage, "%s", LOG_COMPONENT) >= (gint)sizeof componentStorage) {
        g_printerr("logger name is too long\n");
        exit(1);
    }
    captureEnvironment();
}

void About::setName(const char *value)
{
    if (value == NULL) {
        nameStorage[0] = '\0';
        return;
    }
    if (g_snprintf(nameStorage, sizeof nameStorage, "%s", value) >= (gint)sizeof nameStorage) {
        g_printerr("logger name is too long\n");
        exit(1);
    }
}

void About::setLogDir(const char *dir)
{
    logDirValue = dir;
}

void About::setBaseDir(const char *dir)
{
    baseDirValue = dir;
}

void About::setQuiet(bool quiet)
{
    if (quiet) {
        quietMode = true;
    }
}

const char *About::name(void) const { return nameStorage; }
const char *About::description(void) const { return descriptionStorage; }
const char *About::component(void) const { return componentStorage; }
int About::versionMajor(void) const { return versionMajorValue; }
int About::versionMinor(void) const { return versionMinorValue; }
int About::versionPatch(void) const { return versionPatchValue; }
const char *About::logDir(void) const { return logDirValue; }
const char *About::baseDir(void) const { return baseDirValue; }
bool About::quiet(void) const { return quietMode; }
bool About::json(void) const { return jsonMode; }
bool About::debug(void) const { return debugEnabled; }
int About::verboseValue(void) const { return verbose; }
int About::purposeValue(void) const { return purpose; }
int About::pageValue(void) const { return page; }
int About::readerValue(void) const { return reader; }
const char *About::prefixValue(void) const { return prefix; }
const char *About::homeValue(void) const { return home; }
const char *About::datestampValue(void) const { return datestamp; }

static int env_is_one(const char *envName)
{
    const char *value = getenv(envName);

    if (value == NULL) {
        return 0;
    }
    return atoi(value) == 1 || strcmp(value, "1") == 0;
}

static int env_debug_token(void)
{
    const char *value;
    char folded[8];
    size_t n;

    value = getenv("DEBUG");
    if (value == NULL || value[0] == '\0') {
        value = getenv("debug");
    }
    if (value == NULL) {
        return 0;
    }
    n = 0;
    while (value[n] != '\0' && n + 1 < sizeof folded) {
        char c = value[n];

        if (c >= 'A' && c <= 'Z') {
            c = (char)(c - 'A' + 'a');
        }
        folded[n] = c;
        n++;
    }
    if (value[n] != '\0') {
        return 0;
    }
    folded[n] = '\0';
    return strcmp(folded, "show") == 0
        || strcmp(folded, "true") == 0
        || strcmp(folded, "1") == 0;
}

void About::captureEnvironment(void)
{
    if (env_is_one("JSON")) {
        jsonMode = true;
    }
    if (env_is_one("QUIET")) {
        quietMode = true;
    }
    if (env_debug_token()) {
        debugEnabled = true;
    }
}

void About::resetFlags(void)
{
    quietMode = false;
    jsonMode = false;
    debugEnabled = false;
    verbose = 0;
    purpose = 0;
    prefix = NULL;
    home = NULL;
    datestamp = NULL;
    sawQuiet = 0;
    sawJson = 0;
    sawDebug = 0;
    sawVerbose = 0;
    page = 0;
    reader = 0;
    baseStorage[0] = '\0';
    baseDirValue = NULL;
    logDirValue = NULL;
}

static gchar *json_escape_text(const char *text)
{
    GString *out = g_string_new("");
    const unsigned char *cursor = (const unsigned char *)(text ? text : "");

    for (; *cursor != '\0'; cursor++) {
        switch (*cursor) {
            case '\\':
                g_string_append(out, "\\\\");
                break;
            case '"':
                g_string_append(out, "\\\"");
                break;
            case '\n':
                g_string_append(out, "\\n");
                break;
            case '\r':
                g_string_append(out, "\\r");
                break;
            case '\t':
                g_string_append(out, "\\t");
                break;
            default:
                if (*cursor < 0x20) {
                    g_string_append_printf(out, "\\u%04x", *cursor);
                } else {
                    g_string_append_c(out, (gchar)*cursor);
                }
                break;
        }
    }
    return g_string_free(out, FALSE);
}

const char *About::commandName(void) const
{
    if (page == 1) {
        return "help";
    }
    if (page == 2) {
        return "about";
    }
    if (purpose == 1) {
        return "test";
    }
    if (purpose == 2) {
        return "list";
    }
    if (purpose == 3) {
        return "show";
    }
    return "";
}

void About::die(const char *message)
{
    const char *verb = commandName();

    if (jsonMode && verb[0] != '\0') {
        gchar *escaped = json_escape_text(message ? message : "");

        g_print("{\"command\":\"%s\",\"error\":\"%s\"}\n", verb, escaped);
        g_free(escaped);
        exit(1);
    }
    g_printerr("%s\n", message ? message : "");
    exit(1);
}

void About::joinBase(char *out, size_t outSize, const char *head, const char *tail)
{
    gchar *trimmed;
    gsize length;
    gint wrote;

    if (head == NULL || head[0] == '\0' || strchr(head, '\n') != NULL || strchr(head, '\r') != NULL) {
        g_printerr("logger folder directory is empty or not one line\n");
        exit(1);
    }
    trimmed = g_strdup(head);
    length = strlen(trimmed);
    while (length > 1 && trimmed[length - 1] == '/') {
        trimmed[--length] = '\0';
    }
    if (strcmp(trimmed, "/") == 0) {
        wrote = g_snprintf(out, outSize, "/%s", tail);
    } else {
        wrote = g_snprintf(out, outSize, "%s/%s", trimmed, tail);
    }
    g_free(trimmed);
    if (wrote < 0 || (size_t)wrote >= outSize) {
        g_printerr("logger folder path is too long\n");
        exit(1);
    }
}

const char *About::takeValue(int argc, char **argv, int *index, const char *flag)
{
    char message[256];

    if (*index + 1 >= argc || argv[*index + 1][0] == '\0') {
        g_snprintf(message, sizeof message, "missing value for %s", flag);
        die(message);
    }
    *index += 1;
    return argv[*index];
}

void About::setPurpose(int next)
{
    if (purpose != 0 || page != 0) {
        die("one test purpose command");
    }
    purpose = next;
}

void About::setPage(int next)
{
    if (page != 0 || purpose != 0) {
        die("one test purpose command");
    }
    page = next;
}

int About::datestampOk(const char *text) const
{
    if (text == NULL || strlen(text) != 8) {
        return 0;
    }
    for (int i = 0; i < 8; i++) {
        if (text[i] < '0' || text[i] > '9') {
            return 0;
        }
    }
    return 1;
}

// Purpose:
//   Read flags, then the command word. An unknown word is ignored by this scan
//   only when it is not one of the verbs. A rejected test command exits here.
// Parameters:
//   argc and argv are the process arguments.
// Dependencies:
//   The environment JSON, QUIET, and DEBUG are read before the scan.
// Why This Design:
//   The flags stay on this object. The logger does not parse and does not choose the verb.
// CIAO Principles Applied:
//   Caution: a rejected command exits before a log file is created.
// Protection Rule:
//   requirement-cpp-parameters.md. help and about run after the scan.
// Last updated: 2026-11-11
void About::parse(int argc, char **argv)
{
    char tail[256];
    const char *dir;

    resetFlags();
    captureEnvironment();

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--quiet") == 0) {
            sawQuiet = 1;
            quietMode = true;
        } else if (strcmp(argv[i], "--json") == 0) {
            sawJson = 1;
            jsonMode = true;
        } else if (strcmp(argv[i], "--debug") == 0) {
            sawDebug = 1;
            debugEnabled = true;
        } else if (strcmp(argv[i], "--verbose") == 0) {
            sawVerbose = 1;
            verbose = 1;
        } else if (strcmp(argv[i], "--prefix") == 0) {
            prefix = takeValue(argc, argv, &i, "--prefix");
        } else if (strcmp(argv[i], "--home") == 0) {
            home = takeValue(argc, argv, &i, "--home");
        } else if (strcmp(argv[i], "--log-datestamp") == 0) {
            datestamp = takeValue(argc, argv, &i, "--log-datestamp");
        } else if (strcmp(argv[i], "test") == 0) {
            setPurpose(1);
        } else if (strcmp(argv[i], "list") == 0) {
            setPurpose(2);
        } else if (strcmp(argv[i], "show") == 0) {
            setPurpose(3);
        } else if (strcmp(argv[i], "help") == 0) {
            setPage(1);
        } else if (strcmp(argv[i], "about") == 0) {
            setPage(2);
        }
    }

    if (purpose != 0) {
        if (prefix == NULL && home == NULL) {
            die("test purpose command requires --prefix or --home");
        }
        if ((purpose == 1 || purpose == 3) && datestamp == NULL) {
            die("test purpose command requires --log-datestamp YYYYMMDD");
        }
    }
    if (datestamp != NULL && !datestampOk(datestamp)) {
        die("--log-datestamp requires YYYYMMDD");
    }

    reader = (page != 0 || purpose == 2 || purpose == 3) ? 1 : 0;
    if (purpose != 0) {
        dir = prefix != NULL ? prefix : home;
        if (g_snprintf(tail, sizeof tail, "%s/%s",
                       prefix != NULL ? "var" : ".app",
                       nameStorage) >= (gint)sizeof tail) {
            g_printerr("logger folder path is too long\n");
            exit(1);
        }
        joinBase(baseStorage, sizeof baseStorage, dir, tail);
        baseDirValue = baseStorage;
    }
    if (jsonMode) {
        quietMode = true;
    }
}

// Purpose:
//   Parse, bind the logger when the verb records, route one verb, and finish.
// Parameters:
//   argc and argv are the process arguments. The return is the process status.
// Dependencies:
//   Invocation, Help, and AboutCli bind the logger. Logger binds this object.
// Why This Design:
//   The program chooses the verb. The logger records. A reader constructs the logger after parse, so the reader flag is already set.
// CIAO Principles Applied:
//   Caution: help, about, list, and show do not append a log line.
// Protection Rule:
//   The logger does not choose the verb and does not exit for a successful reader verb.
// Last updated: 2026-11-11
int About::run(int argc, char **argv)
{
    parse(argc, argv);

    Logger logger(*this, NULL);
    Invocation invocation;

    invocation.bind(logger);
    invocation.takeCommand();
    if (page == 1) {
        Help help;

        help.bind(logger);
        help.printPage();
        invocation.markComplete();
        return 0;
    }
    if (page == 2) {
        AboutCli cli;

        cli.bind(logger);
        cli.printPage();
        invocation.markComplete();
        return 0;
    }
    if (purpose == 2) {
        logger.logger_list_log_files();
        invocation.markComplete();
        return 0;
    }
    if (purpose == 3) {
        logger.logger_show_log_file();
        invocation.markComplete();
        return 0;
    }
    logger.logger_finish_command();
    if (purpose == 1) {
        logger.logger_info(componentStorage, "test log");
        logger.logger_debug(componentStorage, "component %s end", componentStorage);
        invocation.markComplete();
        return 0;
    }
    logger.logger_info(componentStorage, "Starting %s %d.%d.%d",
                       nameStorage, versionMajorValue, versionMinorValue, versionPatchValue);
    logger.logger_debug(componentStorage, "component %s end", componentStorage);
    logger.logger_info(componentStorage, "Finished %s %d.%d.%d",
                       nameStorage, versionMajorValue, versionMinorValue, versionPatchValue);
    invocation.markComplete();
    return 0;
}
