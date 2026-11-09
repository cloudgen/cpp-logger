// logger.cpp
// =========================================================================
// CIAO DEFENSIVE CODING STYLE - OUTPUT SYSTEM IMPLEMENTATION (GNOME + JSON + DEBUG)
// =========================================================================
//
// Console output. No GTK window.
// g_print and g_printerr are allowed in this file only.
// !!! DO NOT REMOVE OR MODIFY ANY HEADER OR FUNCTION COMMENT BLOCK !!! 
//
// Last aligned with SyncPrjs CIAO style + GNOME + JSON + "help" + DEBUG: 2026-04-12
// =========================================================================

#include "logger.h"
#include "project.h"
#include <glib.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

static bool logger_quiet_mode = false;
static bool logger_json_mode = false;
static bool debug_env_enabled = false;
static int logger_verbose = 0;
static char logger_folder[4096];
static char logger_log_path[4096];
static int logger_purpose = 0;
static const char *logger_prefix_arg = NULL;
static const char *logger_home_arg = NULL;
static const char *logger_datestamp_arg = NULL;
static int logger_saw_quiet = 0;
static int logger_saw_json = 0;
static int logger_saw_debug = 0;
static int logger_saw_verbose = 0;

/* Purpose: join a directory and a relative tail into one base path.
   Parameters: head is the directory. tail is "var/<name>" or ".app/<name>".
   Dependencies: g_strdup, g_snprintf, g_printerr.
   Why This Design: "/" plus "var/<name>" is "/var/<name>", not "//var/<name>".
   CIAO Principles: an empty directory does not fall through to another home.
   Protection Rule: requirement-cpp-logger-folder.md owns the two environments.
   Last updated: 2026-10-03 */
static void logger_join_base(char *out, size_t out_size, const char *head, const char *tail)
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
        wrote = g_snprintf(out, out_size, "/%s", tail);
    } else {
        wrote = g_snprintf(out, out_size, "%s/%s", trimmed, tail);
    }
    g_free(trimmed);
    if (wrote < 0 || (size_t)wrote >= out_size) {
        g_printerr("logger folder path is too long\n");
        exit(1);
    }
}

static int logger_datestamp_ok(const char *text)
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

static void logger_apply_base(const char *base, const char *stamp)
{
    if (g_snprintf(logger_folder, sizeof logger_folder, "%s/log", base) >= (gint)sizeof logger_folder) {
        g_printerr("logger folder path is too long\n");
        exit(1);
    }
    if (g_snprintf(logger_log_path, sizeof logger_log_path, "%s/%s-%s.log",
                   logger_folder, PROJECT_NAME, stamp) >= (gint)sizeof logger_log_path) {
        g_printerr("log file path is too long\n");
        exit(1);
    }
}

int logger_is_test_verb(void)
{
    return logger_purpose == 1;
}

static void logger_set_purpose(int purpose)
{
    if (logger_purpose != 0) {
        g_printerr("one test purpose command\n");
        exit(1);
    }
    logger_purpose = purpose;
}

static gint logger_filename_cmp(gconstpointer left, gconstpointer right)
{
    return strcmp((const char *)left, (const char *)right);
}

static void logger_list_log_files(void)
{
    GDir *dir;
    const char *name;
    GSList *names = NULL;
    GSList *item;
    GError *error = NULL;

    if (logger_folder[0] == '\0') {
        g_printerr("log directory unresolved\n");
        exit(1);
    }
    dir = g_dir_open(logger_folder, 0, &error);
    if (dir == NULL) {
        if (error != NULL && g_error_matches(error, G_FILE_ERROR, G_FILE_ERROR_NOENT)) {
            g_error_free(error);
            return;
        }
        g_printerr("cannot read log directory %s\n", logger_folder);
        if (error != NULL) {
            g_error_free(error);
        }
        exit(1);
    }
    while ((name = g_dir_read_name(dir)) != NULL) {
        char path[4096];

        if (!g_str_has_suffix(name, ".log")) {
            continue;
        }
        if (g_snprintf(path, sizeof path, "%s/%s", logger_folder, name) >= (gint)sizeof path) {
            g_dir_close(dir);
            g_slist_free_full(names, g_free);
            g_printerr("log file path is too long\n");
            exit(1);
        }
        if (!g_file_test(path, G_FILE_TEST_IS_REGULAR)) {
            continue;
        }
        names = g_slist_insert_sorted(names, g_strdup(name), logger_filename_cmp);
    }
    g_dir_close(dir);
    for (item = names; item != NULL; item = item->next) {
        g_print("%s\n", (char *)item->data);
    }
    g_slist_free_full(names, g_free);
}

static void logger_show_log_file(void)
{
    gchar *text = NULL;
    gsize length = 0;
    GError *error = NULL;

    if (logger_log_path[0] == '\0') {
        g_printerr("log file unresolved\n");
        exit(1);
    }
    if (!g_file_get_contents(logger_log_path, &text, &length, &error)) {
        g_printerr("cannot read log file %s\n", logger_log_path);
        if (error != NULL) {
            g_error_free(error);
        }
        exit(1);
    }
    if (length > 0) {
        g_print("%s", text);
    }
    g_free(text);
}

static const char *logger_take_value(int argc, char **argv, int *index, const char *flag)
{
    if (*index + 1 >= argc || argv[*index + 1][0] == '\0') {
        g_printerr("missing value for %s\n", flag);
        exit(1);
    }
    *index += 1;
    return argv[*index];
}

// ──────────────────────────────────────────────
// logger_set_quiet() / logger_get_quiet() / logger_get_debug()
// ──────────────────────────────────────────────
//
// Purpose:
//   logger_set_quiet stores quiet mode. logger_get_quiet returns that flag.
//   logger_get_debug returns the DEBUG flag captured by logger_output_init.
//
// Parameters:
//   Input:
//     - quiet : bool - true hides non-error lines
//   Output/Return:
//     - logger_get_quiet and logger_get_debug return bool
//
// Dependencies:
//   - Data: logger_json_mode, logger_quiet_mode, debug_env_enabled
//   - Headers required: logger.h
//
// Why This Design:
//   Callers change quiet only through logger_set_quiet.
//   JSON mode forces quiet, so the setter cannot turn quiet off while JSON is on.
//   Debug is environment-only. A getter exposes the captured flag. There is no setter.
//
// CIAO Principles Applied:
//   - Caution: JSON mode cannot be undone by the quiet setter
//   - Intentional: one write path for quiet and one read path for debug
//   - Over-protect: the flags are not exported from logger.h
//
// Protection Rule (Sacred):
//   Do not add logger_set_debug.
//   Do not export logger_quiet_mode, logger_json_mode, or debug_env_enabled.
//
// Last updated: 2026-10-03
// ──────────────────────────────────────────────
void logger_set_quiet(bool quiet)
{
    if (logger_json_mode) {
        logger_quiet_mode = true;
        return;
    }
    logger_quiet_mode = quiet;
}

bool logger_get_quiet(void)
{
    return logger_quiet_mode;
}

bool logger_get_debug(void)
{
    return debug_env_enabled;
}

// ──────────────────────────────────────────────
// logger_output_init() - General Purpose Requirement: Initialize Output System
// ──────────────────────────────────────────────
//
// Purpose:
//   Must be called very early in main() to parse command line and environment.
//   This is the Single Point of Truth for application output configuration.
//
// Parameters:
//   Input:
//     - argc : int - Number of command line arguments
//     - argv : char** - Command line argument array
//   Output/Return: None (configures global output flags)
//
// Dependencies:
//   - Functions called: getenv(), atoi(), strncmp(), strcmp(), logger_info(), logger_debug(), logger_print_help()
//   - Headers required: logger.h, project.h, stdlib.h, string.h
//   - External: Parses --quiet, --json, help, and DEBUG, QUIET, JSON.
//
// CIAO Principles Applied:
//   - Caution: Careful parsing with fallbacks and bounds checking
//   - Intentional: Early initialization and clear feature documentation
//   - Anti-fragile: Graceful handling of missing/invalid flags
//   - Over-protect: All output configuration centralized here
//
// Protection Rule (Sacred):
//   Future AI must NOT remove any existing flag/env logic.
//
// Last updated: Full CIAO expansion with Parameters & Dependencies (2026-05-08)
// ──────────────────────────────────────────────
static void logger_log_found_switches(void)
{
    if (!debug_env_enabled) {
        return;
    }
    if (logger_saw_verbose) {
        logger_debug("switch --verbose");
    }
    if (logger_saw_quiet) {
        logger_debug("switch --quiet");
    }
    if (logger_saw_json) {
        logger_debug("switch --json");
    }
    if (logger_saw_debug) {
        logger_debug("switch --debug");
    }
    if (logger_prefix_arg != NULL) {
        logger_debug("switch --prefix %s", logger_prefix_arg);
    }
    if (logger_home_arg != NULL) {
        logger_debug("switch --home %s", logger_home_arg);
    }
    if (logger_datestamp_arg != NULL) {
        logger_debug("switch --log-datestamp %s", logger_datestamp_arg);
    }
}

void logger_output_init(int argc, char **argv)
{
    const char *debug_env;
    int debug_from_env;

    logger_json_mode = false;
    logger_set_quiet(false);
    logger_folder[0] = '\0';
    logger_log_path[0] = '\0';
    logger_purpose = 0;
    logger_verbose = 0;
    logger_prefix_arg = NULL;
    logger_home_arg = NULL;
    logger_datestamp_arg = NULL;
    logger_saw_quiet = 0;
    logger_saw_json = 0;
    logger_saw_debug = 0;
    logger_saw_verbose = 0;

    debug_env = getenv("DEBUG");
    debug_from_env = (debug_env != NULL && *debug_env != '\0');
    debug_env_enabled = debug_from_env;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--quiet") == 0) {
            logger_saw_quiet = 1;
            logger_set_quiet(true);
        } else if (strcmp(argv[i], "--json") == 0) {
            logger_saw_json = 1;
            logger_json_mode = true;
        } else if (strcmp(argv[i], "--debug") == 0) {
            logger_saw_debug = 1;
            debug_env_enabled = true;
        } else if (strcmp(argv[i], "--verbose") == 0) {
            logger_saw_verbose = 1;
            logger_verbose = 1;
        } else if (strcmp(argv[i], "--prefix") == 0) {
            logger_prefix_arg = logger_take_value(argc, argv, &i, "--prefix");
        } else if (strcmp(argv[i], "--home") == 0) {
            logger_home_arg = logger_take_value(argc, argv, &i, "--home");
        } else if (strcmp(argv[i], "--log-datestamp") == 0) {
            logger_datestamp_arg = logger_take_value(argc, argv, &i, "--log-datestamp");
        } else if (strcmp(argv[i], "test") == 0) {
            logger_set_purpose(1);
        } else if (strcmp(argv[i], "list") == 0) {
            logger_set_purpose(2);
        } else if (strcmp(argv[i], "show") == 0) {
            logger_set_purpose(3);
        } else if (strcmp(argv[i], "help") == 0) {
            logger_print_help();
            exit(0);
        } else if (strcmp(argv[i], "about") == 0) {
            logger_print_about();
            exit(0);
        }
    }

    if (!logger_get_quiet()) {
        const char *q = getenv("QUIET");
        if (q && (atoi(q) == 1 || strcmp(q, "1") == 0))
            logger_set_quiet(true);
    }
    if (!logger_json_mode) {
        const char *j = getenv("JSON");
        if (j && (atoi(j) == 1 || strcmp(j, "1") == 0))
            logger_json_mode = true;
    }

    if (logger_purpose != 0) {
        if (logger_prefix_arg == NULL && logger_home_arg == NULL) {
            g_printerr("test purpose command requires --prefix or --home\n");
            exit(1);
        }
        if ((logger_purpose == 1 || logger_purpose == 3) && logger_datestamp_arg == NULL) {
            g_printerr("test purpose command requires --log-datestamp YYYYMMDD\n");
            exit(1);
        }
    }
    if (logger_datestamp_arg != NULL && !logger_datestamp_ok(logger_datestamp_arg)) {
        g_printerr("--log-datestamp requires YYYYMMDD\n");
        exit(1);
    }

    {
        char today[16];
        char base[4096];
        char tail[256];
        const char *stamp = logger_datestamp_arg;
        if (stamp == NULL) {
            time_t now = time(NULL);
            struct tm *tm_ptr = localtime(&now);
            if (tm_ptr == NULL || strftime(today, sizeof today, "%Y%m%d", tm_ptr) == 0) {
                g_strlcpy(today, "19700101", sizeof today);
            }
            stamp = today;
        }
        if (logger_purpose != 0) {
            const char *dir = logger_prefix_arg != NULL ? logger_prefix_arg : logger_home_arg;
            if (g_snprintf(tail, sizeof tail, "%s/%s",
                           logger_prefix_arg != NULL ? "var" : ".app",
                           PROJECT_NAME) >= (gint)sizeof tail) {
                g_printerr("logger folder path is too long\n");
                exit(1);
            }
            logger_join_base(base, sizeof base, dir, tail);
            logger_apply_base(base, stamp);
        } else if (geteuid() == 0) {
            if (g_snprintf(base, sizeof base, "/var/%s", PROJECT_NAME) >= (gint)sizeof base) {
                g_printerr("logger folder path is too long\n");
                exit(1);
            }
            logger_apply_base(base, stamp);
        } else {
            const char *home = getenv("HOME");
            if (home != NULL && home[0] != '\0') {
                if (g_snprintf(tail, sizeof tail, ".app/%s", PROJECT_NAME) >= (gint)sizeof tail) {
                    g_printerr("logger folder path is too long\n");
                    exit(1);
                }
                logger_join_base(base, sizeof base, home, tail);
                logger_apply_base(base, stamp);
            }
        }
    }

    if (logger_purpose == 2) {
        logger_list_log_files();
        exit(0);
    }
    if (logger_purpose == 3) {
        logger_show_log_file();
        exit(0);
    }

    logger_log_found_switches();
    logger_debug("component %s start", LOG_COMPONENT);

    if (logger_json_mode) {
        logger_set_quiet(true);
        logger_debug("JSON mode active → quiet mode forced ON");
    }

    if (debug_env_enabled) {
        if (logger_folder[0] != '\0') {
            logger_debug("logger folder %s", logger_folder);
            logger_debug("log file %s", logger_log_path);
        } else {
            logger_debug("logger folder unresolved");
        }
    }

    if (!logger_get_quiet() && !logger_json_mode) {
        logger_info("%s Output System initialized", PROJECT_NAME);
        if (debug_from_env) {
            logger_info("Debug mode enabled via DEBUG=1 environment variable");
        }
    } else if (logger_json_mode) {
        logger_info("Output system initialized in JSON mode");
    }
}

// ──────────────────────────────────────────────
// logger_log() - Core Single Source of Output Function
// ──────────────────────────────────────────────
//
// Purpose:
//   ALL output in the project must pass through this function.
//   This is the Single Source of Truth for all console output.
//
// Parameters:
//   Input:
//     - level  : LoggerOutputLevel - Message severity
//     - format : const char* - printf-style format string
//     - ap     : va_list (by value) - Variable arguments
//   Output/Return: None (prints to stdout/stderr or JSON)
//
// Dependencies:
//   - Functions called: g_strdup_vprintf(), g_print(), g_printerr(), time(), va_copy(), va_end()
//   - Headers required: glib.h, stdarg.h, time.h, logger.h
//
// Critical va_list Rule (Security & Correctness):
//   This function receives a va_list BY VALUE (not by pointer, not via ...).
//   Inside this function we MUST use va_copy() before consuming the list
//   because g_strdup_vprintf() (and similar GLib functions) will advance
//   the va_list, leaving the original caller’s list in an indeterminate state.
//
//   NEVER change the signature to accept "..." again and call va_start here.
//   NEVER pass a va_list from a convenience wrapper without va_copy().
//
//   Violating this causes:
//     - "incorrectly passing a va_list" static analysis warnings
//     - Undefined behavior on many ABIs (especially x86_64 and aarch64)
//     - Potential format-string related security issues
//
// Why This Must Stay Strong:
//   Prevents fragmentation of output logic (common failure in previous versions).
//   Supports both human-readable and JSON structured output.
//   Maintains strict GNOME compliance (only g_print / g_printerr allowed).
//
// Protection Rule (Sacred):
//   Do NOT bypass this function.
//   Do NOT replace g_print/g_printerr with stdio functions.
//   Do NOT modify the va_list handling logic without updating this comment.
//   This is the heart of the CIAO Single Source of Output principle.
//
// Last updated: Full CIAO expansion with Parameters & Dependencies (2026-05-08)
// ──────────────────────────────────────────────
/* Purpose: keep a quote, backslash, or newline inside one JSON string.
   Parameters: text is the formatted message. Returns a new buffer.
   Dependencies: g_string_new, g_string_append, g_string_free.
   Why This Design: JSON mode must not print a second, non-JSON fragment.
   CIAO Principles: Caution on control bytes. Over-protect one object per line.
   Protection Rule: requirement-cpp-json.md owns this escape.
   Last updated: 2026-10-03 */
static gchar *logger_json_escape(const char *text)
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

static const char *logger_level_name(LoggerOutputLevel level)
{
    switch (level) {
        case LOGGER_LEVEL_INFO:    return "INFO";
        case LOGGER_LEVEL_WARNING: return "WARNING";
        case LOGGER_LEVEL_ERROR:   return "ERROR";
        case LOGGER_LEVEL_DEBUG:   return "DEBUG";
        case LOGGER_LEVEL_FATAL:   return "FATAL";
        default:                 return "INFO";
    }
}

static int logger_stderr_level(LoggerOutputLevel level)
{
    return level == LOGGER_LEVEL_ERROR || level == LOGGER_LEVEL_FATAL;
}

/* ChronicleLogger log_message: local time, pid, level, component, then the message.
   ERROR and FATAL go to stderr. INFO, WARNING, and DEBUG go to stdout.
   CRITICAL is not a well-known level and is not emitted.
   The component is LOG_COMPONENT ("main"): " @main" before " :] ". */
static void logger_emit_chronicle(LoggerOutputLevel level, const char *message)
{
    char timestamp[32];
    time_t now = time(NULL);
    struct tm *tm_ptr;
    const char *text = message ? message : "";
    const char *level_name = logger_level_name(level);

    tm_ptr = localtime(&now);
    if (tm_ptr == NULL ||
        strftime(timestamp, sizeof timestamp, "%Y-%m-%d %H:%M:%S", tm_ptr) == 0) {
        g_strlcpy(timestamp, "1970-01-01 00:00:00", sizeof timestamp);
    }

    if (logger_stderr_level(level)) {
        g_printerr("[%s] pid:%ld [%s] @%s :] %s\n",
                   timestamp, (long)getpid(), level_name, LOG_COMPONENT, text);
    } else {
        g_print("[%s] pid:%ld [%s] @%s :] %s\n",
                timestamp, (long)getpid(), level_name, LOG_COMPONENT, text);
    }
}

/* Purpose: append the same human line to the dated log file.
   Parameters: level and message are the event just emitted.
   Dependencies: g_mkdir_with_parents, fopen. No mkstemp.
   Why This Design: the test reads one file path from requirement-cpp-logger.md.
   Protection Rule: do not write when the log path is unresolved.
   Last updated: 2026-10-03 */
static void logger_append_log_file(LoggerOutputLevel level, const char *message)
{
    char timestamp[32];
    time_t now;
    struct tm *tm_ptr;
    FILE *file;
    const char *text = message ? message : "";

    if (logger_log_path[0] == '\0' || logger_folder[0] == '\0') {
        return;
    }
    if (g_mkdir_with_parents(logger_folder, 0755) != 0) {
        g_printerr("cannot create logger folder %s\n", logger_folder);
        if (logger_purpose != 0) {
            exit(1);
        }
        return;
    }
    now = time(NULL);
    tm_ptr = localtime(&now);
    if (tm_ptr == NULL ||
        strftime(timestamp, sizeof timestamp, "%Y-%m-%d %H:%M:%S", tm_ptr) == 0) {
        g_strlcpy(timestamp, "1970-01-01 00:00:00", sizeof timestamp);
    }
    file = fopen(logger_log_path, "a");
    if (file == NULL) {
        g_printerr("cannot open log file %s\n", logger_log_path);
        if (logger_purpose != 0) {
            exit(1);
        }
        return;
    }
    fprintf(file, "[%s] pid:%ld [%s] @%s :] %s\n",
            timestamp, (long)getpid(), logger_level_name(level), LOG_COMPONENT, text);
    fclose(file);
}

void logger_log(LoggerOutputLevel level, const char *format, va_list ap)   // Changed: va_list (by value), no ...
{
    if (level == LOGGER_LEVEL_DEBUG && !logger_get_debug()) {
        return;
    }

    /* Stdout human lines stay hidden unless --verbose or debug mode is set.
       Neither one shows them while quiet or JSON is on. ERROR and FATAL stay
       on stderr. The dated file still receives the human line. */
    // Make a copy because g_strdup_vprintf / g_vasprintf may consume the list
    va_list ap_copy;
    va_copy(ap_copy, ap);

    gchar *message = g_strdup_vprintf(format, ap_copy);

    logger_append_log_file(level, message);

    if (logger_json_mode) {
        gchar *escaped = logger_json_escape(message ? message : "");
        g_print("{\"timestamp\":%ld,\"level\":\"%s\",\"message\":\"%s\"}\n",
                (long)time(NULL),
                logger_level_name(level),
                escaped);
        g_free(escaped);
    } else if (logger_stderr_level(level) ||
               (!logger_get_quiet() && (logger_verbose || debug_env_enabled))) {
        logger_emit_chronicle(level, message);
    }

    g_free(message);
    va_end(ap_copy);
}

// ──────────────────────────────────────────────
// logger_info() - Convenience Function: logger_info
// ──────────────────────────────────────────────
//
// Purpose:
//   Standard information messages
//
// Parameters:
//   Input:
//     - format : const char* - printf-style format string
//     - ...    : Variable arguments
//   Output/Return: None
//
// Dependencies:
//   - Functions called: logger_log()
//   - Headers required: logger.h
//
// Protection:
//   Routes through logger_log() to maintain Single Source of Output
//
// Last updated: Full CIAO expansion (2026-05-08)
// ──────────────────────────────────────────────
void logger_info(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    logger_log(LOGGER_LEVEL_INFO, format, args);
    va_end(args);
}

// ──────────────────────────────────────────────
// logger_warn() - Convenience Function: logger_warn
// ──────────────────────────────────────────────
//
// Purpose:
//   Warning messages (stderr)
//
// Parameters:
//   Input:
//     - format : const char* - printf-style format string
//     - ...    : Variable arguments
//   Output/Return: None
//
// Dependencies:
//   - Functions called: logger_log()
//   - Headers required: logger.h
//
// Protection:
//   Routes through logger_log()
//
// Last updated: Full CIAO expansion (2026-05-08)
// ──────────────────────────────────────────────
void logger_warn(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    logger_log(LOGGER_LEVEL_WARNING, format, args);
    va_end(args);
}

// ──────────────────────────────────────────────
// logger_error() - Convenience Function: logger_error
// ──────────────────────────────────────────────
//
// Purpose:
//   Error messages (always shown, even in --quiet mode)
//
// Parameters:
//   Input:
//     - format : const char* - printf-style format string
//     - ...    : Variable arguments
//   Output/Return: None
//
// Dependencies:
//   - Functions called: logger_log()
//   - Headers required: logger.h
//
// Protection:
//   Critical path - must never be suppressed
//
// Last updated: Full CIAO expansion (2026-05-08)
// ──────────────────────────────────────────────
void logger_error(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    logger_log(LOGGER_LEVEL_ERROR, format, args);
    va_end(args);
}

void logger_fatal(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    logger_log(LOGGER_LEVEL_FATAL, format, args);
    va_end(args);
}

// ──────────────────────────────────────────────
// logger_debug() - Debug Function: logger_debug
// ──────────────────────────────────────────────
//
// Purpose:
//   Developer-only debug output.
//
// Parameters:
//   Input:
//     - format : const char* - printf-style format string
//     - ...    : Variable arguments
//   Output/Return: None
//
// Dependencies:
//   - Functions called: logger_log()
//   - Headers required: logger.h
//
// Rules:
//   - Only active when DEBUG=1 (or any non-empty value) in environment.
//   - Quiet hides the human debug line.
//   - JSON mode still emits that call as one JSON object.
//
// Why This Exists:
//   Prevents debug spam in normal runs while allowing deep inspection when needed.
//
// Last updated: Full CIAO expansion (2026-05-08)
// ──────────────────────────────────────────────
void logger_debug(const char *format, ...)
{
    if (!logger_get_debug()) return;

    va_list args;
    va_start(args, format);
    logger_log(LOGGER_LEVEL_DEBUG, format, args);
    va_end(args);
}

// ──────────────────────────────────────────────
// logger_print_help() - Help Function: logger_print_help
// ──────────────────────────────────────────────
//
// Purpose:
//   Show usage when user types "help"
//
// Parameters:
//   Input: None
//   Output/Return: None
//
// Dependencies:
//   - Functions called: g_print()
//   - Headers required: glib.h
//
// Protection:
//   Uses g_print only (GNOME compliant)
//
// Last updated: Full CIAO expansion (2026-05-08)
// ──────────────────────────────────────────────
void logger_print_help(void)
{
    g_print("Usage: %s [help|about|test|list|show] [options]\n\n", PROJECT_NAME);
    g_print("Commands:\n");
    g_print("  help                 Show this help message\n");
    g_print("  about                Show the name, version, and description\n");
    g_print("  test                 Write one test log line. Requires --prefix or --home, and --log-datestamp\n");
    g_print("  list                 List regular *.log names. Requires --prefix or --home\n");
    g_print("  show                 Print one log file. Requires --prefix or --home, and --log-datestamp\n");
    g_print("  One of test, list, or show. A second command stops.\n\n");
    g_print("Options:\n");
    g_print("  --verbose            Show stdout log lines. Ignored with --quiet or --json\n");
    g_print("  --quiet              Suppress non-error messages. --verbose does not override this\n");
    g_print("  --json               JSON mode (implies quiet; only JSON lines). --verbose does not add a human line\n");
    g_print("  --debug              Enable debug events and show stdout log lines. Hidden with --quiet or --json\n");
    g_print("  --prefix <dir>       Test switch for root or sudo. Log directory <dir>/var/%s/log\n", PROJECT_NAME);
    g_print("  --home <dir>         Test switch for any other user. Log directory <dir>/.app/%s/log\n", PROJECT_NAME);
    g_print("  --log-datestamp <YYYYMMDD>  Log file %s-<YYYYMMDD>.log\n\n", PROJECT_NAME);
    g_print("Environments:\n");
    g_print("  root or sudo         Base /var/%s and log directory /var/%s/log\n", PROJECT_NAME, PROJECT_NAME);
    g_print("  other                Base $HOME/.app/%s and log directory $HOME/.app/%s/log\n\n", PROJECT_NAME, PROJECT_NAME);
    g_print("Environment Variables:\n");
    g_print("  DEBUG=1              Enable logger_debug() output\n");
    g_print("  QUIET=1              Same as --quiet\n");
    g_print("  JSON=1               Same as --json\n");
    g_print("  HOME                 Other-user base on a normal run. A test purpose command ignores it\n\n");
    g_print("Examples:\n");
    g_print("  DEBUG=1 %s\n", PROJECT_NAME);
    g_print("  %s test --home /tmp --log-datestamp 20261003 --debug\n", PROJECT_NAME);
    g_print("  %s test --prefix /tmp --log-datestamp 20261003 --debug\n", PROJECT_NAME);
    g_print("  %s list --home /tmp\n", PROJECT_NAME);
    g_print("  %s show --home /tmp --log-datestamp 20261003\n", PROJECT_NAME);
    g_print("  %s --verbose\n", PROJECT_NAME);
    g_print("  %s --quiet\n", PROJECT_NAME);
    g_print("  %s --json\n", PROJECT_NAME);
    g_print("  %s about\n", PROJECT_NAME);
}

void logger_print_about(void)
{
    g_print("%s %d.%d.%d\n", PROJECT_NAME, VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH);
    g_print("%s\n", PROJECT_DESCRIPTION);
    g_print("No database. No configuration file.\n");
}

