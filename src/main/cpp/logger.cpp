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
#include <sys/stat.h>
#include <sys/utsname.h>
#include <sys/wait.h>

/* Purpose: the object main constructed, for a C caller with no object parameter.
   The retain driver calls logger_maintain_directory before any construction.
   That wrapper builds a short-lived object and does not store it here.
   Protection Rule: requirement-cpp-oop.md. This pointer is not a factory. */
Logger *Logger::logger_active = NULL;


const char *Logger::logger_command_name(void)
{
    if (logger_pending_page == 1) {
        return "help";
    }
    if (logger_pending_page == 2) {
        return "about";
    }
    if (logger_purpose == 1) {
        return "test";
    }
    if (logger_purpose == 2) {
        return "list";
    }
    if (logger_purpose == 3) {
        return "show";
    }
    return "";
}

/* Purpose: one verb failure. JSON mode prints one object. Otherwise stderr.
   Parameters: message is the existing sentence.
   Dependencies: logger_json_escape, logger_command_name.
   Why This Design: the sentence stays the same. The channel follows JSON mode.
   CIAO Principles: a verb result is one object, with no human line beside it.
   Protection Rule: requirement-cpp-json.md owns the object.
   Last updated: 2026-11-10 */
void Logger::logger_die(const char *message)
{
    const char *name = logger_command_name();

    if (logger_json_mode && name[0] != '\0') {
        gchar *escaped = logger_json_escape(message ? message : "");

        g_print("{\"command\":\"%s\",\"error\":\"%s\"}\n", name, escaped);
        g_free(escaped);
        exit(1);
    }
    g_printerr("%s\n", message ? message : "");
    exit(1);
}

/* Purpose: join a directory and a relative tail into one base path.
   Parameters: head is the directory. tail is "var/<name>" or ".app/<name>".
   Dependencies: g_strdup, g_snprintf, g_printerr.
   Why This Design: "/" plus "var/<name>" is "/var/<name>", not "//var/<name>".
   CIAO Principles: an empty directory does not fall through to another home.
   Protection Rule: requirement-cpp-logger-folder.md owns the two environments.
   Last updated: 2026-10-03 */
void Logger::logger_join_base(char *out, size_t out_size, const char *head, const char *tail)
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

int Logger::logger_datestamp_ok(const char *text)
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

void Logger::logger_apply_base(const char *base, const char *stamp)
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

int Logger::logger_is_test_verb(void)
{
    return logger_purpose == 1;
}

void Logger::logger_set_purpose(int purpose)
{
    if (logger_purpose != 0 || logger_pending_page != 0) {
        logger_die("one test purpose command");
    }
    logger_purpose = purpose;
}

void Logger::logger_list_emit(GSList *names)
{
    GSList *item;

    if (!logger_json_mode) {
        for (item = names; item != NULL; item = item->next) {
            g_print("%s\n", (char *)item->data);
        }
        return;
    }

    {
        GString *out = g_string_new("");
        gchar *folder = logger_json_escape(logger_folder);
        int first = 1;

        g_string_append_printf(out, "{\"command\":\"list\",\"folder\":\"%s\",\"files\":[", folder);
        g_free(folder);
        for (item = names; item != NULL; item = item->next) {
            gchar *escaped = logger_json_escape((char *)item->data);

            if (!first) {
                g_string_append_c(out, ',');
            }
            first = 0;
            g_string_append_printf(out, "\"%s\"", escaped);
            g_free(escaped);
        }
        g_string_append(out, "]}\n");
        g_print("%s", out->str);
        g_string_free(out, TRUE);
    }
}

gint Logger::logger_filename_cmp(gconstpointer left, gconstpointer right)
{
    return strcmp((const char *)left, (const char *)right);
}

void Logger::logger_list_log_files(void)
{
    GDir *dir;
    const char *name;
    GSList *names = NULL;
    GError *error = NULL;

    if (logger_folder[0] == '\0') {
        logger_die("log directory unresolved");
    }
    dir = g_dir_open(logger_folder, 0, &error);
    if (dir == NULL) {
        if (error != NULL && g_error_matches(error, G_FILE_ERROR, G_FILE_ERROR_NOENT)) {
            g_error_free(error);
            if (logger_json_mode) {
                logger_list_emit(NULL);
            }
            return;
        }
        {
            char message[4600];

            g_snprintf(message, sizeof message, "cannot read log directory %s", logger_folder);
            if (error != NULL) {
                g_error_free(error);
            }
            logger_die(message);
        }
    }
    while ((name = g_dir_read_name(dir)) != NULL) {
        char path[4096];

        if (!g_str_has_suffix(name, ".log")) {
            continue;
        }
        if (g_snprintf(path, sizeof path, "%s/%s", logger_folder, name) >= (gint)sizeof path) {
            g_dir_close(dir);
            g_slist_free_full(names, g_free);
            logger_die("log file path is too long");
        }
        if (!g_file_test(path, G_FILE_TEST_IS_REGULAR)) {
            continue;
        }
        names = g_slist_insert_sorted(names, g_strdup(name), logger_filename_cmp);
    }
    g_dir_close(dir);
    logger_list_emit(names);
    g_slist_free_full(names, g_free);
}

void Logger::logger_show_log_file(void)
{
    gchar *text = NULL;
    gsize length = 0;
    GError *error = NULL;

    if (logger_log_path[0] == '\0') {
        logger_die("log file unresolved");
    }
    if (!g_file_get_contents(logger_log_path, &text, &length, &error)) {
        char message[4600];

        g_snprintf(message, sizeof message, "cannot read log file %s", logger_log_path);
        if (error != NULL) {
            g_error_free(error);
        }
        logger_die(message);
    }
    if (logger_json_mode) {
        gchar *folder = logger_json_escape(logger_log_path);
        gchar *body = logger_json_escape(text ? text : "");

        g_print("{\"command\":\"show\",\"file\":\"%s\",\"content\":\"%s\"}\n", folder, body);
        g_free(folder);
        g_free(body);
        g_free(text);
        return;
    }
    if (length > 0) {
        g_print("%s", text);
    }
    g_free(text);
}

const char *Logger::logger_take_value(int argc, char **argv, int *index, const char *flag)
{
    if (*index + 1 >= argc || argv[*index + 1][0] == '\0') {
        char message[256];

        g_snprintf(message, sizeof message, "missing value for %s", flag);
        logger_die(message);
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
void Logger::logger_set_quiet(bool quiet)
{
    if (logger_json_mode) {
        logger_quiet_mode = true;
        return;
    }
    logger_quiet_mode = quiet;
}

bool Logger::logger_get_quiet(void)
{
    return logger_quiet_mode;
}

bool Logger::logger_get_debug(void)
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
void Logger::logger_log_found_switches(void)
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

Logger::Logger(int argc, char **argv)
    : logger_quiet_mode(false),
      logger_json_mode(false),
      debug_env_enabled(false),
      logger_verbose(0),
      logger_folder(),
      logger_log_path(),
      logger_purpose(0),
      logger_prefix_arg(NULL),
      logger_home_arg(NULL),
      logger_datestamp_arg(NULL),
      logger_saw_quiet(0),
      logger_saw_json(0),
      logger_saw_debug(0),
      logger_saw_verbose(0),
      logger_pending_page(0),
      logger_retention_done(0),
      logger_maintain_busy(0)
{
    /* argc < 0 builds an unbound object for logger_maintain_directory.
       The retain driver has no argv for this constructor. Parsing would
       print and could append a log. This path does not store logger_active. */
    if (argc < 0) {
        return;
    }
    logger_active = this;
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
    logger_retention_done = 0;

    debug_env = getenv("DEBUG");
    debug_from_env = (debug_env != NULL && *debug_env != '\0');
    debug_env_enabled = debug_from_env;

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
            if (logger_pending_page != 0 || logger_purpose != 0) {
                logger_die("one test purpose command");
            }
            logger_pending_page = 1;
        } else if (strcmp(argv[i], "about") == 0) {
            if (logger_pending_page != 0 || logger_purpose != 0) {
                logger_die("one test purpose command");
            }
            logger_pending_page = 2;
        }
    }

    if (logger_pending_page == 1) {
        logger_print_help();
        exit(0);
    }
    if (logger_pending_page == 2) {
        logger_print_about();
        exit(0);
    }

    if (logger_purpose != 0) {
        if (logger_prefix_arg == NULL && logger_home_arg == NULL) {
            logger_die("test purpose command requires --prefix or --home");
        }
        if ((logger_purpose == 1 || logger_purpose == 3) && logger_datestamp_arg == NULL) {
            logger_die("test purpose command requires --log-datestamp YYYYMMDD");
        }
    }
    if (logger_datestamp_arg != NULL && !logger_datestamp_ok(logger_datestamp_arg)) {
        logger_die("--log-datestamp requires YYYYMMDD");
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
gchar *Logger::logger_json_escape(const char *text)
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

const char *Logger::logger_level_name(LoggerOutputLevel level)
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

int Logger::logger_stderr_level(LoggerOutputLevel level)
{
    return level == LOGGER_LEVEL_ERROR || level == LOGGER_LEVEL_FATAL;
}

/* ChronicleLogger log_message: local time, pid, level, component, then the message.
   ERROR and FATAL go to stderr. INFO, WARNING, and DEBUG go to stdout.
   CRITICAL is not a well-known level and is not emitted.
   The component is LOG_COMPONENT ("main"): " @main" before " :] ". */
void Logger::logger_emit_chronicle(LoggerOutputLevel level, const char *message)
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

/* Purpose: remove one regular file. A directory and a symlink stay.
   Parameters: path is one absolute or relative file path.
   Dependencies: lstat, unlink.
   Why This Design: housekeeping must not remove a directory that shares a log name.
   CIAO Principles: Caution on the file type. A missing path is not an error.
   Protection Rule: requirement-cpp-log-retention.md. No recursive delete.
   Last updated: 2026-11-10 */
void Logger::logger_unlink_regular(const char *path)
{
    struct stat st;

    if (path == NULL || path[0] == '\0') {
        return;
    }
    if (lstat(path, &st) != 0 || !S_ISREG(st.st_mode)) {
        return;
    }
    unlink(path);
}

/* Purpose: join a log directory and one file name.
   Parameters: out receives the path. Returns 0 when the path does not fit.
   Dependencies: g_snprintf.
   Why This Design: a too-long path is skipped. The stored log line stays stored.
   CIAO Principles: housekeeping does not exit.
   Protection Rule: requirement-cpp-log-retention.md.
   Last updated: 2026-11-10 */
int Logger::logger_retention_join(char *out, size_t out_size, const char *folder, const char *name)
{
    gint wrote;

    wrote = g_snprintf(out, out_size, "%s/%s", folder, name);
    return wrote > 0 && (size_t)wrote < out_size;
}

/* Purpose: read eight digits as a local calendar date.
   Parameters: stamp is YYYYMMDD. date receives a valid GDate.
   Dependencies: g_date_valid_dmy, g_date_set_dmy.
   Why This Design: 20260230 is not a day, so that name stays.
   CIAO Principles: an unreadable date is not evidence to delete.
   Protection Rule: requirement-cpp-log-retention.md rule 3.
   Last updated: 2026-11-10 */
int Logger::logger_stamp_to_date(const char *stamp, GDate *date)
{
    unsigned year;
    unsigned month;
    unsigned day;

    if (!logger_datestamp_ok(stamp)) {
        return 0;
    }
    year = (unsigned)(stamp[0] - '0') * 1000u + (unsigned)(stamp[1] - '0') * 100u
         + (unsigned)(stamp[2] - '0') * 10u + (unsigned)(stamp[3] - '0');
    month = (unsigned)(stamp[4] - '0') * 10u + (unsigned)(stamp[5] - '0');
    day = (unsigned)(stamp[6] - '0') * 10u + (unsigned)(stamp[7] - '0');
    if (!g_date_valid_dmy((GDateDay)day, (GDateMonth)month, (GDateYear)year)) {
        return 0;
    }
    g_date_clear(date, 1);
    g_date_set_dmy(date, (GDateDay)day, (GDateMonth)month, (GDateYear)year);
    return g_date_valid(date);
}

/* Purpose: classify one directory entry as a dated plain file or package.
   Parameters: name is the basename. stamp receives eight digits. Returns 1 or 0.
   Dependencies: PROJECT_NAME.
   Why This Design: notes.txt and a short stamp are not this notebook's days.
   CIAO Principles: only the dated name for this product is eligible.
   Protection Rule: requirement-cpp-log-retention.md rule 3.
   Last updated: 2026-11-10 */
int Logger::logger_retention_stamp(const char *name, char stamp[9])
{
    size_t prefix_len;
    const char *tail;

    if (name == NULL) {
        return 0;
    }
    prefix_len = strlen(PROJECT_NAME);
    if (strncmp(name, PROJECT_NAME, prefix_len) != 0 || name[prefix_len] != '-') {
        return 0;
    }
    tail = name + prefix_len + 1;
    for (int i = 0; i < 8; i++) {
        if (tail[i] < '0' || tail[i] > '9') {
            return 0;
        }
    }
    memcpy(stamp, tail, 8);
    stamp[8] = '\0';
    tail += 8;
    if (strcmp(tail, ".log") == 0 || strcmp(tail, ".log.tar.gz") == 0 || strcmp(tail, ".log.zip") == 0) {
        return 1;
    }
    return 0;
}

/* Purpose: run tar or zip and report a zero exit.
   Parameters: argv is the tool and its arguments.
   Dependencies: g_spawn_sync. The tool comes from PATH.
   Why This Design: this program does not link a compression library.
   CIAO Principles: a missing tool is a failed package, not a failed log write.
   Protection Rule: requirement-cpp-log-retention.md rule 9. No mkstemp.
   Last updated: 2026-11-10 */
int Logger::logger_spawn_ok(gchar **argv)
{
    gint status = 1;
    GError *error = NULL;
    gboolean spawned;
    GSpawnFlags flags;

    flags = (GSpawnFlags)(G_SPAWN_SEARCH_PATH | G_SPAWN_STDOUT_TO_DEV_NULL | G_SPAWN_STDERR_TO_DEV_NULL);
    spawned = g_spawn_sync(NULL, argv, NULL, flags, NULL, NULL, NULL, NULL, &status, &error);
    if (error != NULL) {
        g_error_free(error);
    }
    if (!spawned) {
        return 0;
    }
    return WIFEXITED(status) && WEXITSTATUS(status) == 0;
}

/* Purpose: write one complete package, then remove the plain file.
   Parameters: folder, stamp, and the plain basename.
   Dependencies: tar and gzip on this host. zip on Windows.
   Why This Design: the package is renamed into place only after the tool exits 0.
   CIAO Principles: a partial package is removed. The plain file stays when that happens.
   Protection Rule: requirement-cpp-log-retention.md rules 5 and 9.
   Last updated: 2026-11-10 */
int Logger::logger_compress_day(const char *folder, const char *stamp, const char *plain_name)
{
    char plain_path[4096];
    char final_path[4096];
    char partial_path[4096];
    char other_path[4096];
    char final_name[256];
    char partial_name[256];
    char other_name[256];
    struct stat st;
    gchar *argv[8];

    if (g_snprintf(final_name, sizeof final_name,
#ifdef _WIN32
                   "%s-%s.log.zip",
#else
                   "%s-%s.log.tar.gz",
#endif
                   PROJECT_NAME, stamp) >= (gint)sizeof final_name) {
        return 0;
    }
    if (g_snprintf(partial_name, sizeof partial_name,
#ifdef _WIN32
                   "%s-%s.log.partial.zip",
#else
                   "%s-%s.log.tar.gz.partial",
#endif
                   PROJECT_NAME, stamp) >= (gint)sizeof partial_name) {
        return 0;
    }
    if (g_snprintf(other_name, sizeof other_name,
#ifdef _WIN32
                   "%s-%s.log.tar.gz",
#else
                   "%s-%s.log.zip",
#endif
                   PROJECT_NAME, stamp) >= (gint)sizeof other_name) {
        return 0;
    }
    if (!logger_retention_join(plain_path, sizeof plain_path, folder, plain_name) ||
        !logger_retention_join(final_path, sizeof final_path, folder, final_name) ||
        !logger_retention_join(partial_path, sizeof partial_path, folder, partial_name) ||
        !logger_retention_join(other_path, sizeof other_path, folder, other_name)) {
        return 0;
    }
    if (lstat(final_path, &st) == 0 && S_ISREG(st.st_mode) && st.st_size > 0) {
        logger_unlink_regular(plain_path);
        logger_unlink_regular(other_path);
        return 1;
    }
    if (lstat(plain_path, &st) != 0 || !S_ISREG(st.st_mode)) {
        return 0;
    }
    logger_unlink_regular(final_path);
    logger_unlink_regular(partial_path);
#ifdef _WIN32
    argv[0] = (gchar *)"zip";
    argv[1] = (gchar *)"-j";
    argv[2] = (gchar *)"-q";
    argv[3] = partial_path;
    argv[4] = plain_path;
    argv[5] = NULL;
#else
    argv[0] = (gchar *)"tar";
    argv[1] = (gchar *)"-czf";
    argv[2] = partial_path;
    argv[3] = (gchar *)"-C";
    argv[4] = (gchar *)folder;
    argv[5] = (gchar *)plain_name;
    argv[6] = NULL;
#endif
    if (!logger_spawn_ok(argv)) {
        logger_unlink_regular(partial_path);
        return 0;
    }
    if (rename(partial_path, final_path) != 0) {
        logger_unlink_regular(partial_path);
        return 0;
    }
    if (lstat(final_path, &st) != 0 || !S_ISREG(st.st_mode) || st.st_size == 0) {
        logger_unlink_regular(final_path);
        return 0;
    }
    logger_unlink_regular(plain_path);
    logger_unlink_regular(other_path);
    return 1;
}

/* Purpose: package or remove one dated day.
   Parameters: folder, the active plain basename, today's date, and the stamp.
   Dependencies: logger_compress_day, logger_unlink_regular, g_date_days_between.
   Why This Design: count 7 stays plain, count 30 stays one package, count 31 is gone.
   CIAO Principles: the active file of this run stays plain.
   Protection Rule: requirement-cpp-log-retention.md rules 4 through 7.
   Last updated: 2026-11-10 */
void Logger::logger_maintain_one(const char *folder, const char *active_basename, const GDate *today, const char *stamp)
{
    char plain_name[256];
    char path[4096];
    char extra_name[256];
    GDate log_date;
    gint span;
    const char *suffixes[] = {
        ".log",
        ".log.tar.gz",
        ".log.zip",
        ".log.tar.gz.partial",
        ".log.zip.partial",
        ".log.partial.zip",
        NULL
    };
    int index;

    if (!logger_stamp_to_date(stamp, &log_date)) {
        return;
    }
    if (g_snprintf(plain_name, sizeof plain_name, "%s-%s.log", PROJECT_NAME, stamp) >= (gint)sizeof plain_name) {
        return;
    }
    if (active_basename != NULL && strcmp(active_basename, plain_name) == 0) {
        return;
    }
    span = g_date_days_between(&log_date, today);
    if (span <= 7) {
        return;
    }
    if (span <= 30) {
        logger_compress_day(folder, stamp, plain_name);
        return;
    }
    for (index = 0; suffixes[index] != NULL; index++) {
        if (g_snprintf(extra_name, sizeof extra_name, "%s-%s%s", PROJECT_NAME, stamp, suffixes[index]) >= (gint)sizeof extra_name) {
            continue;
        }
        if (!logger_retention_join(path, sizeof path, folder, extra_name)) {
            continue;
        }
        logger_unlink_regular(path);
    }
}

/* Purpose: walk one log directory and apply the two windows.
   Parameters: folder, active plain basename, and today's YYYYMMDD.
   Dependencies: g_dir_open. Does not create the directory.
   Why This Design: the same walk serves a normal run and the retention check.
   CIAO Principles: a missing directory stays missing. Housekeeping does not exit.
   Protection Rule: requirement-cpp-log-retention.md rules 8 and 9.
   Last updated: 2026-11-10 */
void Logger::logger_maintain_body(const char *folder, const char *active_basename, const char *today_yyyymmdd)
{
    GDate today;
    GDir *dir;
    const char *name;
    GHashTable *stamps;
    GList *keys;
    GList *item;

    if (folder == NULL || folder[0] == '\0' || strchr(folder, '\n') != NULL || strchr(folder, '\r') != NULL) {
        return;
    }
    if (active_basename == NULL) {
        active_basename = "";
    }
    if (!logger_stamp_to_date(today_yyyymmdd, &today)) {
        return;
    }
    if (!g_file_test(folder, G_FILE_TEST_IS_DIR)) {
        return;
    }
    dir = g_dir_open(folder, 0, NULL);
    if (dir == NULL) {
        return;
    }
    stamps = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, NULL);
    while ((name = g_dir_read_name(dir)) != NULL) {
        char stamp[9];

        if (!logger_retention_stamp(name, stamp)) {
            continue;
        }
        if (!g_hash_table_contains(stamps, stamp)) {
            g_hash_table_insert(stamps, g_strdup(stamp), GINT_TO_POINTER(1));
        }
    }
    g_dir_close(dir);
    keys = g_hash_table_get_keys(stamps);
    for (item = keys; item != NULL; item = item->next) {
        logger_maintain_one(folder, active_basename, &today, (const char *)item->data);
    }
    g_list_free(keys);
    g_hash_table_destroy(stamps);
}

void Logger::logger_maintain_directory(const char *folder, const char *active_basename, const char *today_yyyymmdd)
{
    if (logger_maintain_busy) {
        return;
    }
    logger_maintain_busy = 1;
    logger_maintain_body(folder, active_basename, today_yyyymmdd);
    logger_maintain_busy = 0;
}

/* Purpose: append the same human line to the dated log file.
   Parameters: level and message are the event just emitted.
   Dependencies: g_mkdir_with_parents, fopen. No mkstemp.
   Why This Design: the test reads one file path from requirement-cpp-logger.md.
   Protection Rule: do not write when the log path is unresolved.
   Last updated: 2026-10-03 */
void Logger::logger_append_log_file(LoggerOutputLevel level, const char *message)
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
    if (logger_purpose == 0 && !logger_retention_done) {
        char today[16];
        const char *active;
        time_t now_day;
        struct tm *day_tm;

        logger_retention_done = 1;
        now_day = time(NULL);
        day_tm = localtime(&now_day);
        if (day_tm != NULL && strftime(today, sizeof today, "%Y%m%d", day_tm) != 0) {
            active = strrchr(logger_log_path, '/');
            active = (active != NULL) ? active + 1 : logger_log_path;
            logger_maintain_directory(logger_folder, active, today);
        }
    }
}

void Logger::logger_log(LoggerOutputLevel level, const char *format, va_list ap)   // Changed: va_list (by value), no ...
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
void Logger::logger_info(const char *format, ...)
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
void Logger::logger_warn(const char *format, ...)
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
void Logger::logger_error(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    logger_log(LOGGER_LEVEL_ERROR, format, args);
    va_end(args);
}

void Logger::logger_fatal(const char *format, ...)
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
void Logger::logger_debug(const char *format, ...)
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
void Logger::logger_print_help(void)
{
    if (logger_json_mode) {
        g_print("{\"command\":\"help\",\"verbs\":[\"help\",\"about\",\"test\",\"list\",\"show\"],\"flags\":[\"--verbose\",\"--quiet\",\"--json\",\"--debug\",\"--prefix\",\"--home\",\"--log-datestamp\"]}\n");
        return;
    }
    g_print("Usage: %s [help|about|test|list|show] [options]\n\n", PROJECT_NAME);
    g_print("Commands:\n");
    g_print("  help                 Show this help message\n");
    g_print("  about                Show this program, this computer, and where it is running from\n");
    g_print("  test                 Write one test log line. Requires --prefix or --home, and --log-datestamp\n");
    g_print("  list                 List regular *.log names. Requires --prefix or --home\n");
    g_print("  show                 Print one log file. Requires --prefix or --home, and --log-datestamp\n");
    g_print("  One of test, list, or show. A second command stops.\n\n");
    g_print("Options:\n");
    g_print("  --verbose            Show stdout log lines. Ignored with --quiet or --json\n");
    g_print("  --quiet              Suppress non-error messages. --verbose does not override this\n");
    g_print("  --json               JSON mode. Each verb prints one JSON object. --verbose does not add a human line\n");
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

// ──────────────────────────────────────────────
// logger_print_about() - About page: identity, host check, install location
// ──────────────────────────────────────────────
//
// Purpose:
//   Print three live facts for this run: which program this is, what this
//   computer is, and where the program is running from.
//
// Parameters:
//   Input: None. Reads the environment and path existence at print time.
//   Output/Return: None
//
// Dependencies:
//   - Functions called: g_print(), uname(), readlink(), getenv(), stat(), access()
//   - Headers required: project.h, glib.h, sys/utsname.h, sys/stat.h, unistd.h
//
// Protection:
//   Uses g_print only. Does not spawn a compiler, an interpreter, or a package manager.
//   Does not install. Does not execute the global or local bin to read a version.
//   Does not create a cache directory, a persistence directory, or a log directory.
//
// Last updated: Three live facts (2026-11-10)
// ──────────────────────────────────────────────
void Logger::logger_about_copy(char *out, size_t out_size, const char *value)
{
    if (value == NULL || value[0] == '\0') {
        out[0] = '\0';
        return;
    }
    g_strlcpy(out, value, out_size);
}

void Logger::logger_about_probe(char *out, size_t out_size, const char *path, const char *version)
{
    struct stat st;

    if (path == NULL || path[0] == '\0') {
        g_strlcpy(out, "not found", out_size);
        return;
    }
    if (stat(path, &st) == 0 && S_ISREG(st.st_mode) && access(path, X_OK) == 0) {
        g_strlcpy(out, version, out_size);
        return;
    }
    g_strlcpy(out, "not found", out_size);
}

void Logger::logger_print_about(void)
{
    char stamp[64];
    char version[32];
    char os_line[256];
    char arch[128];
    char user[256];
    char shell[256];
    char location[4096];
    char preferred[512];
    char fallback1[512];
    char used[512];
    char persistence[512];
    char global_path[512];
    char local_path[512];
    char global_value[32];
    char local_value[32];
    const char *prefix;
    time_t now;
    struct tm *local_tm;
    struct utsname uts;
    struct stat st;
    ssize_t link_len;
    const char *home;
    FILE *docker;
    int root;
    int inside_docker;

    g_snprintf(version, sizeof(version), "%d.%d.%d",
               VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH);

    stamp[0] = '\0';
    now = time(NULL);
    if (now != (time_t)-1) {
        local_tm = localtime(&now);
        if (local_tm != NULL &&
            strftime(stamp, sizeof(stamp), "%Y-%m-%d %H:%M:%S", local_tm) > 0) {
            g_strlcat(stamp, ".000000", sizeof(stamp));
        } else {
            stamp[0] = '\0';
        }
    }

    os_line[0] = '\0';
    arch[0] = '\0';
    if (uname(&uts) == 0) {
        g_snprintf(os_line, sizeof(os_line), "%s %s", uts.sysname, uts.release);
        g_strlcpy(arch, uts.machine, sizeof(arch));
    }

    logger_about_copy(user, sizeof(user), getenv("USER"));
    logger_about_copy(shell, sizeof(shell), getenv("SHELL"));

    link_len = readlink("/proc/self/exe", location, sizeof(location) - 1);
    if (link_len < 0) {
        g_strlcpy(location, PROJECT_NAME, sizeof(location));
    } else {
        location[link_len] = '\0';
    }

    preferred[0] = '\0';
    fallback1[0] = '\0';
    used[0] = '\0';
    persistence[0] = '\0';
    home = getenv("HOME");
    if (home != NULL && home[0] == '\0') {
        home = NULL;
    }
    root = (geteuid() == 0);
    if (root) {
        g_snprintf(preferred, sizeof(preferred), "/var/cache/%s", PROJECT_NAME);
        g_strlcpy(used, preferred, sizeof(used));
        g_snprintf(persistence, sizeof(persistence), "/var/%s", PROJECT_NAME);
    } else {
        g_snprintf(preferred, sizeof(preferred), "/dev/shm/cache/cache-%s", PROJECT_NAME);
        if (home != NULL) {
            g_snprintf(fallback1, sizeof(fallback1), "%s/.cache/cache-%s", home, PROJECT_NAME);
            g_snprintf(persistence, sizeof(persistence), "%s/.app/%s", home, PROJECT_NAME);
        }
        if (stat("/dev/shm", &st) == 0 && S_ISDIR(st.st_mode)) {
            g_strlcpy(used, preferred, sizeof(used));
        } else {
            g_strlcpy(used, fallback1, sizeof(used));
        }
    }

    inside_docker = 0;
    docker = fopen("/.dockerenv", "r");
    if (docker != NULL) {
        fclose(docker);
        inside_docker = 1;
    }

    prefix = getenv("PREFIX");
    if (prefix != NULL && prefix[0] == '\0') {
        prefix = NULL;
    }
    if (prefix != NULL) {
        g_snprintf(global_path, sizeof(global_path), "%s/bin/%s", prefix, PROJECT_NAME);
    } else {
        g_snprintf(global_path, sizeof(global_path), "/usr/local/bin/%s", PROJECT_NAME);
    }
    local_path[0] = '\0';
    if (home != NULL) {
        g_snprintf(local_path, sizeof(local_path), "%s/.local/bin/%s", home, PROJECT_NAME);
    }
    logger_about_probe(global_value, sizeof(global_value), global_path, version);
    logger_about_probe(local_value, sizeof(local_value), local_path, version);

    if (logger_json_mode) {
        GString *out = g_string_new("{");
        gchar *escaped;
        char compiler[160];
        int first = 1;

        g_snprintf(compiler, sizeof compiler, "g++ %s", __VERSION__);

        #define LOGGER_JSON_STR(key, value) \
            do { \
                escaped = logger_json_escape(value ? (value) : ""); \
                if (!first) { \
                    g_string_append_c(out, ','); \
                } \
                first = 0; \
                g_string_append_printf(out, "\"%s\":\"%s\"", (key), escaped); \
                g_free(escaped); \
            } while (0)

        LOGGER_JSON_STR("command", "about");
        LOGGER_JSON_STR("name", PROJECT_NAME);
        LOGGER_JSON_STR("version", version);
        LOGGER_JSON_STR("description", PROJECT_DESCRIPTION);
        LOGGER_JSON_STR("domain", "No database. No configuration file.");
        LOGGER_JSON_STR("runtime_tools", "none");
        LOGGER_JSON_STR("entry_points", PROJECT_NAME);
        g_string_append(out, ",\"host\":{");
        first = 1;
        LOGGER_JSON_STR("stamp", stamp);
        LOGGER_JSON_STR("compiler", compiler);
        LOGGER_JSON_STR("cpp_standard", "C++17");
        LOGGER_JSON_STR("operation_system", os_line);
        LOGGER_JSON_STR("architecture", arch);
        LOGGER_JSON_STR("current_user", user);
        LOGGER_JSON_STR("shell", shell);
        if (!first) {
            g_string_append_c(out, ',');
        }
        g_string_append_printf(out, "\"inside_docker\":%s", inside_docker ? "true" : "false");
        first = 0;
        LOGGER_JSON_STR("location", location);
        if (!first) {
            g_string_append_c(out, ',');
        }
        g_string_append_printf(out, "\"pid\":%ld", (long)getpid());
        first = 0;
        LOGGER_JSON_STR("cache_folder_used", used);
        LOGGER_JSON_STR("cache_folder_preferred", preferred);
        LOGGER_JSON_STR("cache_folder_1st_fallback", fallback1);
        LOGGER_JSON_STR("cache_folder_2nd_fallback", "");
        LOGGER_JSON_STR("persistence_storage", persistence);
        if (!first) {
            g_string_append_c(out, ',');
        }
        g_string_append_printf(out, "\"tty\":%s", isatty(STDOUT_FILENO) ? "true" : "false");
        g_string_append(out, "},\"install\":{");
        first = 1;
        LOGGER_JSON_STR("global_path", global_path);
        LOGGER_JSON_STR("global", global_value);
        LOGGER_JSON_STR("local_path", local_path);
        LOGGER_JSON_STR("local", local_value);
        LOGGER_JSON_STR("running_from", location);
        g_string_append(out, "}}\n");
        #undef LOGGER_JSON_STR
        g_print("%s", out->str);
        g_string_free(out, TRUE);
        return;
    }

    g_print("%s %s\n", PROJECT_NAME, version);
    g_print("%s\n", PROJECT_DESCRIPTION);
    g_print("Domain: No database. No configuration file.\n");
    g_print("Runtime tools: none\n");
    g_print("Entry points: %s\n", PROJECT_NAME);
    g_print("\n");
    g_print("%s %s(v%s)  [CHECK SYSTEM]:\n", stamp, PROJECT_NAME, version);
    g_print("  Now checking your operation system!\n");
    g_print("    Compiler: g++ %s\n", __VERSION__);
    g_print("    C++ standard: C++17\n");
    g_print("    Operation System: %s\n", os_line);
    g_print("    Architecture: %s\n", arch);
    g_print("    Current User: %s\n", user);
    g_print("    Shell: %s\n", shell);
    g_print("    Inside docker container: %s\n", inside_docker ? "True" : "False");
    g_print("    Location: %s\n", location);
    g_print("    PID: %ld\n", (long)getpid());
    g_print("    Cache folder used: %s\n", used);
    g_print("    Cache folder (preferred): %s\n", preferred);
    g_print("    Cache folder (1st fallback): %s\n", fallback1);
    g_print("    Cache folder (2nd fallback): %s\n", "");
    g_print("    Persistence storage: %s\n", persistence);
    g_print("    TTY / Interactive: %s\n", isatty(STDOUT_FILENO) ? "yes" : "no");
    g_print("\n");
    g_print("Install location\n");
    g_print("    Global install (%s): %s\n", global_path, global_value);
    if (local_path[0] != '\0') {
        g_print("    Local install (%s): %s\n", local_path, local_value);
    } else {
        g_print("    Local install: %s\n", local_value);
    }
    g_print("    Running from: %s\n", location);
}


/* Purpose: C linkage for the names already declared in logger.h.
   Each name forwards to the object main constructed.
   logger_maintain_directory, with nothing bound, uses a short-lived object
   so the retain driver does not parse argv or append a log.
   Protection Rule: requirement-cpp-oop.md. These names are not new behavior. */

extern "C" void logger_log(LoggerOutputLevel level, const char *format, va_list ap)
{
    if (Logger::logger_active == NULL) {
        g_printerr("Logger is constructed at the call site\n");
        exit(1);
    }
    Logger::logger_active->logger_log(level, format, ap);
}

extern "C" void logger_output_init(int, char **)
{
    if (Logger::logger_active == NULL) {
        g_printerr("Logger is constructed at the call site\n");
        exit(1);
    }
}

extern "C" int logger_is_test_verb(void)
{
    if (Logger::logger_active == NULL) {
        g_printerr("Logger is constructed at the call site\n");
        exit(1);
    }
    return Logger::logger_active->logger_is_test_verb();
}

extern "C" void logger_set_quiet(bool quiet)
{
    if (Logger::logger_active == NULL) {
        g_printerr("Logger is constructed at the call site\n");
        exit(1);
    }
    Logger::logger_active->logger_set_quiet(quiet);
}

extern "C" bool logger_get_quiet(void)
{
    if (Logger::logger_active == NULL) {
        g_printerr("Logger is constructed at the call site\n");
        exit(1);
    }
    return Logger::logger_active->logger_get_quiet();
}

extern "C" bool logger_get_debug(void)
{
    if (Logger::logger_active == NULL) {
        g_printerr("Logger is constructed at the call site\n");
        exit(1);
    }
    return Logger::logger_active->logger_get_debug();
}

extern "C" void logger_info(const char *format, ...)
{
    va_list ap;

    if (Logger::logger_active == NULL) {
        g_printerr("Logger is constructed at the call site\n");
        exit(1);
    }
    va_start(ap, format);
    Logger::logger_active->logger_log(LOGGER_LEVEL_INFO, format, ap);
    va_end(ap);
}

extern "C" void logger_warn(const char *format, ...)
{
    va_list ap;

    if (Logger::logger_active == NULL) {
        g_printerr("Logger is constructed at the call site\n");
        exit(1);
    }
    va_start(ap, format);
    Logger::logger_active->logger_log(LOGGER_LEVEL_WARNING, format, ap);
    va_end(ap);
}

extern "C" void logger_error(const char *format, ...)
{
    va_list ap;

    if (Logger::logger_active == NULL) {
        g_printerr("Logger is constructed at the call site\n");
        exit(1);
    }
    va_start(ap, format);
    Logger::logger_active->logger_log(LOGGER_LEVEL_ERROR, format, ap);
    va_end(ap);
}

extern "C" void logger_fatal(const char *format, ...)
{
    va_list ap;

    if (Logger::logger_active == NULL) {
        g_printerr("Logger is constructed at the call site\n");
        exit(1);
    }
    va_start(ap, format);
    Logger::logger_active->logger_log(LOGGER_LEVEL_FATAL, format, ap);
    va_end(ap);
}

extern "C" void logger_debug(const char *format, ...)
{
    va_list ap;

    if (Logger::logger_active == NULL) {
        g_printerr("Logger is constructed at the call site\n");
        exit(1);
    }
    va_start(ap, format);
    Logger::logger_active->logger_log(LOGGER_LEVEL_DEBUG, format, ap);
    va_end(ap);
}

extern "C" void logger_maintain_directory(const char *folder, const char *active_basename, const char *today_yyyymmdd)
{
    if (Logger::logger_active == NULL) {
        Logger cold(-1, NULL);
        cold.logger_maintain_directory(folder, active_basename, today_yyyymmdd);
        return;
    }
    Logger::logger_active->logger_maintain_directory(folder, active_basename, today_yyyymmdd);
}

extern "C" void logger_print_help(void)
{
    if (Logger::logger_active == NULL) {
        g_printerr("Logger is constructed at the call site\n");
        exit(1);
    }
    Logger::logger_active->logger_print_help();
}

extern "C" void logger_print_about(void)
{
    if (Logger::logger_active == NULL) {
        g_printerr("Logger is constructed at the call site\n");
        exit(1);
    }
    Logger::logger_active->logger_print_about();
}
