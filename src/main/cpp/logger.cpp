// logger.cpp
// =========================================================================
// CIAO DEFENSIVE CODING STYLE - OUTPUT SYSTEM IMPLEMENTATION (GNOME + JSON + DEBUG)
// =========================================================================
//
// Console output. No GTK window.
// g_print and g_printerr are allowed in this file for log lines and help.
// about.cpp prints the about page. Log lines stay in this file.
// !!! DO NOT REMOVE OR MODIFY ANY HEADER OR FUNCTION COMMENT BLOCK !!! 
//
// Last aligned with SyncPrjs CIAO style + GNOME + JSON + "help" + DEBUG: 2026-04-12
// =========================================================================

#include "logger.h"
#include "about.h"
#include "aboutcli.h"
#include "help.h"
#include "project.h"
#include <glib.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>

/* Purpose: the object main constructed, for a C caller with no object parameter.
   The retain driver calls logger_maintain_directory before any construction.
   That wrapper builds a short-lived object and does not store it here.
   Protection Rule: requirement-cpp-oop.md. This pointer is not a factory. */
Logger *Logger::logger_active = NULL;



const char *Logger::logger_command_name(void)
{
    if (bound->page == 1) {
        return "help";
    }
    if (bound->page == 2) {
        return "about";
    }
    if (bound->purpose == 1) {
        return "test";
    }
    if (bound->purpose == 2) {
        return "list";
    }
    if (bound->purpose == 3) {
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

    if (bound->jsonMode && name[0] != '\0') {
        gchar *escaped = logger_json_escape(message ? message : "");

        g_print("{\"command\":\"%s\",\"error\":\"%s\"}\n", name, escaped);
        g_free(escaped);
        exit(1);
    }
    g_printerr("%s\n", message ? message : "");
    exit(1);
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

static void join_base_or_exit(char *out, size_t out_size, const char *head, const char *tail)
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

/* Purpose: join a directory and a relative tail into one base path.
   Parameters: head is the directory. tail is "var/<name>" or ".app/<name>".
   Dependencies: g_strdup, g_snprintf, g_printerr.
   Why This Design: "/" plus "var/<name>" is "/var/<name>", not "//var/<name>".
   CIAO Principles: an empty directory does not fall through to another home.
   Protection Rule: requirement-cpp-logger-folder.md owns the two environments.
   Last updated: 2026-10-03 */
void Logger::logger_join_base(char *out, size_t out_size, const char *head, const char *tail)
{
    join_base_or_exit(out, out_size, head, tail);
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
                   logger_folder, logger_name, stamp) >= (gint)sizeof logger_log_path) {
        g_printerr("log file path is too long\n");
        exit(1);
    }
}

int Logger::logger_is_test_verb(void)
{
    return bound->purpose == 1;
}

void Logger::logger_set_purpose(int purpose)
{
    if (bound->purpose != 0 || bound->page != 0) {
        logger_die("one test purpose command");
    }
    bound->purpose = purpose;
}

void Logger::logger_list_emit(GSList *names)
{
    GSList *item;

    if (!bound->jsonMode) {
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
            if (bound->jsonMode) {
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
    if (bound->jsonMode) {
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
//     - quiet : bool - true hides every human console line
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
//   Debug is --debug or DEBUG/debug set to show, true, or 1. A getter exposes the captured flag. There is no setter.
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
    if (bound->jsonMode) {
        bound->quietMode = true;
        return;
    }
    bound->quietMode = quiet;
}

bool Logger::logger_get_quiet(void)
{
    return bound->quietMode;
}

bool Logger::logger_get_debug(void)
{
    return bound->debugEnabled;
}

bool Logger::logger_get_json(void)
{
    return bound->jsonMode;
}

/* Purpose: return the resolved log directory.
   That directory is the persistence folder plus /log, or an explicit log directory.
   An unresolved directory is an empty string. This read does not create it.
   The about page does not print this return as Persistence storage. */
const char *Logger::logger_persistence_storage(void)
{
    return logger_folder;
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
    if (!bound->debugEnabled) {
        return;
    }
    if (bound->sawVerbose) {
        logger_debug(LOG_COMPONENT, "switch --verbose");
    }
    if (bound->sawQuiet) {
        logger_debug(LOG_COMPONENT, "switch --quiet");
    }
    if (bound->sawJson) {
        logger_debug(LOG_COMPONENT, "switch --json");
    }
    if (bound->sawDebug) {
        logger_debug(LOG_COMPONENT, "switch --debug");
    }
    if (bound->prefix != NULL) {
        logger_debug(LOG_COMPONENT, "switch --prefix %s", bound->prefix);
    }
    if (bound->home != NULL) {
        logger_debug(LOG_COMPONENT, "switch --home %s", bound->home);
    }
    if (bound->datestamp != NULL) {
        logger_debug(LOG_COMPONENT, "switch --log-datestamp %s", bound->datestamp);
    }
}

Logger::Logger(About &about, LoggerClock clock)
    : bound(&about),
      logger_folder(),
      logger_log_path(),
      logger_retention_done(0),
      logger_maintain_busy(0),
      logger_name(),
      logger_old_log_path(),
      logger_clock(NULL)
{
    /* An empty name builds an unbound object. The retain driver uses that
       object for logger_maintain_directory. This path does not store
       logger_active, does not create a directory, and does not write. */
    if (about.name()[0] == '\0') {
        return;
    }

    logger_store_name(about.name());
    logger_clock = clock;
    logger_folder[0] = '\0';
    logger_log_path[0] = '\0';
    logger_old_log_path[0] = '\0';
    if (about.json()) {
        logger_set_quiet(true);
    }

    logger_resolve_paths(about.logDir(), about.baseDir());
    logger_ensure_log_directory();
    logger_active = this;
}

About &Logger::boundAbout(void)
{
    return *bound;
}

void Logger::logger_store_name(const char *logname)
{
    int internal = 0;
    size_t used = 0;
    char built[256];

    for (const char *cursor = logname; *cursor != '\0'; cursor++) {
        if (cursor != logname && *cursor >= 'A' && *cursor <= 'Z') {
            internal = 1;
            break;
        }
    }
    if (!internal) {
        if (g_snprintf(logger_name, sizeof logger_name, "%s", logname) >= (gint)sizeof logger_name) {
            g_printerr("logger name is too long\n");
            exit(1);
        }
        return;
    }
    for (const char *cursor = logname; *cursor != '\0'; cursor++) {
        char letter = *cursor;

        if (cursor != logname && letter >= 'A' && letter <= 'Z') {
            if (used + 2 >= sizeof built) {
                g_printerr("logger name is too long\n");
                exit(1);
            }
            built[used++] = '-';
        }
        if (letter >= 'A' && letter <= 'Z') {
            letter = (char)(letter - 'A' + 'a');
        }
        if (used + 1 >= sizeof built) {
            g_printerr("logger name is too long\n");
            exit(1);
        }
        built[used++] = letter;
    }
    built[used] = '\0';
    memcpy(logger_name, built, used + 1);
}

void Logger::logger_clock_day(char *stamp, size_t stamp_size)
{
    time_t now;
    struct tm *tm_ptr;

    now = logger_clock != NULL ? logger_clock() : time(NULL);
    if (now == (time_t)-1) {
        g_strlcpy(stamp, "19700101", stamp_size);
        return;
    }
    tm_ptr = localtime(&now);
    if (tm_ptr == NULL || strftime(stamp, stamp_size, "%Y%m%d", tm_ptr) == 0) {
        g_strlcpy(stamp, "19700101", stamp_size);
    }
}

void Logger::logger_file_stamp(char *stamp, size_t stamp_size)
{
    if (bound->datestamp != NULL) {
        g_strlcpy(stamp, bound->datestamp, stamp_size);
        return;
    }
    logger_clock_day(stamp, stamp_size);
}

void Logger::logger_format_timestamp(char *timestamp, size_t timestamp_size)
{
    time_t now;
    struct tm *tm_ptr;

    now = logger_clock != NULL ? logger_clock() : time(NULL);
    if (now == (time_t)-1) {
        g_strlcpy(timestamp, "1970-01-01 00:00:00", timestamp_size);
        return;
    }
    tm_ptr = localtime(&now);
    if (tm_ptr == NULL || strftime(timestamp, timestamp_size, "%Y-%m-%d %H:%M:%S", tm_ptr) == 0) {
        g_strlcpy(timestamp, "1970-01-01 00:00:00", timestamp_size);
    }
}

void Logger::logger_apply_folder(const char *folder, const char *stamp)
{
    if (g_snprintf(logger_folder, sizeof logger_folder, "%s", folder) >= (gint)sizeof logger_folder) {
        g_printerr("logger folder path is too long\n");
        exit(1);
    }
    if (g_snprintf(logger_log_path, sizeof logger_log_path, "%s/%s-%s.log",
                   logger_folder, logger_name, stamp) >= (gint)sizeof logger_log_path) {
        g_printerr("log file path is too long\n");
        exit(1);
    }
}

int Logger::logger_try_env_base(const char *env_name, const char *stamp)
{
    const char *value;
    char base[4096];
    char tail[256];

    value = getenv(env_name);
    if (value == NULL || value[0] == '\0') {
        return 0;
    }
    if (g_snprintf(tail, sizeof tail, ".app/%s", logger_name) >= (gint)sizeof tail) {
        g_printerr("logger folder path is too long\n");
        exit(1);
    }
    logger_join_base(base, sizeof base, value, tail);
    logger_apply_base(base, stamp);
    return 1;
}

void Logger::logger_resolve_paths(const char *logdir, const char *basedir)
{
    char stamp[16];
    char base[4096];

    logger_file_stamp(stamp, sizeof stamp);
    if (logdir != NULL && logdir[0] != '\0') {
        logger_apply_folder(logdir, stamp);
        return;
    }
    if (basedir != NULL && basedir[0] != '\0') {
        logger_apply_base(basedir, stamp);
        return;
    }
    if (logger_try_env_base("CONDA_PREFIX", stamp)) {
        return;
    }
    if (logger_try_env_base("PYENV_VIRTUAL_ENV", stamp)) {
        return;
    }
    if (logger_try_env_base("VIRTUAL_ENV", stamp)) {
        return;
    }
    if (geteuid() == 0) {
        if (g_snprintf(base, sizeof base, "/var/%s", logger_name) >= (gint)sizeof base) {
            g_printerr("logger folder path is too long\n");
            exit(1);
        }
        logger_apply_base(base, stamp);
        return;
    }
    logger_try_env_base("HOME", stamp);
}

void Logger::logger_ensure_log_directory(void)
{
    FILE *file;

    if (bound->reader || logger_folder[0] == '\0') {
        return;
    }
    if (!g_file_test(logger_folder, G_FILE_TEST_IS_DIR)) {
        if (g_mkdir_with_parents(logger_folder, 0755) == 0 &&
            !logger_get_quiet() && !bound->jsonMode) {
            g_print("Created directory: %s\n", logger_folder);
        }
    }
    if (logger_log_path[0] == '\0') {
        return;
    }
    file = fopen(logger_log_path, "a");
    if (file == NULL) {
        return;
    }
    fputc('\n', file);
    fclose(file);
}

void Logger::logger_note_dated_path(void)
{
    char today[16];
    char using_message[4600];
    const char *active;

    if (logger_log_path[0] == '\0' || logger_folder[0] == '\0') {
        return;
    }
    if (strcmp(logger_old_log_path, logger_log_path) == 0) {
        return;
    }
    g_strlcpy(logger_old_log_path, logger_log_path, sizeof logger_old_log_path);
    logger_retention_done = 1;
    logger_clock_day(today, sizeof today);
    active = strrchr(logger_log_path, '/');
    active = (active != NULL) ? active + 1 : logger_log_path;
    logger_maintain_directory(logger_folder, active, today);
    if (!bound->debugEnabled) {
        return;
    }
    g_snprintf(using_message, sizeof using_message, "Using %s", logger_log_path);
    logger_append_log_file(LOGGER_LEVEL_INFO, "logger", using_message);
    if (bound->jsonMode) {
        gchar *escaped = logger_json_escape(using_message);

        g_print("{\"timestamp\":%ld,\"level\":\"INFO\",\"message\":\"%s\"}\n",
                (long)time(NULL),
                escaped);
        g_free(escaped);
    } else if (!logger_get_quiet() && (bound->verbose || bound->debugEnabled)) {
        logger_emit_chronicle(LOGGER_LEVEL_INFO, "logger", using_message);
    }
}

void Logger::logger_finish_command(void)
{
    if (bound->page != 0 || bound->purpose == 2 || bound->purpose == 3) {
        return;
    }

    logger_log_found_switches();
    logger_debug(LOG_COMPONENT, "component %s start", LOG_COMPONENT);
    if (bound->jsonMode) {
        logger_set_quiet(true);
        logger_debug(LOG_COMPONENT, "JSON mode active → quiet mode forced ON");
    }
    if (bound->debugEnabled) {
        if (logger_folder[0] != '\0') {
            logger_debug(LOG_COMPONENT, "logger folder %s", logger_folder);
            logger_debug(LOG_COMPONENT, "log file %s", logger_log_path);
        } else {
            logger_debug(LOG_COMPONENT, "logger folder unresolved");
        }
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
    return json_escape_text(text);
}

const char *Logger::logger_level_name(LoggerOutputLevel level)
{
    switch (level) {
        case LOGGER_LEVEL_INFO:    return "INFO";
        case LOGGER_LEVEL_WARNING: return "WARNING";
        case LOGGER_LEVEL_ERROR:    return "ERROR";
        case LOGGER_LEVEL_CRITICAL: return "CRITICAL";
        case LOGGER_LEVEL_FATAL:    return "FATAL";
        case LOGGER_LEVEL_DEBUG:    return "DEBUG";
        default:                    return "INFO";
    }
}

int Logger::logger_stderr_level(LoggerOutputLevel level)
{
    return level == LOGGER_LEVEL_ERROR || level == LOGGER_LEVEL_CRITICAL || level == LOGGER_LEVEL_FATAL;
}

/* ChronicleLogger log_message: local time, pid, level, optional component, then the message.
   ERROR, CRITICAL, and FATAL go to stderr. INFO, WARNING, and DEBUG go to stdout.
   A non-empty component is " @name" before " :] ". An empty component omits that slot. */
void Logger::logger_emit_chronicle(LoggerOutputLevel level, const char *component, const char *message)
{
    char timestamp[32];
    const char *text = message ? message : "";
    const char *part = component ? component : "";
    const char *level_name = logger_level_name(level);

    logger_format_timestamp(timestamp, sizeof timestamp);
    if (logger_stderr_level(level)) {
        if (part[0] != '\0') {
            g_printerr("[%s] pid:%ld [%s] @%s :] %s\n",
                       timestamp, (long)getpid(), level_name, part, text);
        } else {
            g_printerr("[%s] pid:%ld [%s] :] %s\n",
                       timestamp, (long)getpid(), level_name, text);
        }
    } else if (part[0] != '\0') {
        g_print("[%s] pid:%ld [%s] @%s :] %s\n",
                timestamp, (long)getpid(), level_name, part, text);
    } else {
        g_print("[%s] pid:%ld [%s] :] %s\n",
                timestamp, (long)getpid(), level_name, text);
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
void Logger::logger_append_log_file(LoggerOutputLevel level, const char *component, const char *message)
{
    char timestamp[32];
    FILE *file;
    const char *text = message ? message : "";
    const char *part = component ? component : "";

    if (logger_log_path[0] == '\0' || logger_folder[0] == '\0') {
        return;
    }
    if (g_mkdir_with_parents(logger_folder, 0755) != 0) {
        g_printerr("cannot create logger folder %s\n", logger_folder);
        if (bound->purpose != 0) {
            exit(1);
        }
        return;
    }
    logger_format_timestamp(timestamp, sizeof timestamp);
    file = fopen(logger_log_path, "a");
    if (file == NULL) {
        g_printerr("cannot open log file %s\n", logger_log_path);
        if (bound->purpose != 0) {
            exit(1);
        }
        return;
    }
    if (part[0] != '\0') {
        fprintf(file, "[%s] pid:%ld [%s] @%s :] %s\n",
                timestamp, (long)getpid(), logger_level_name(level), part, text);
    } else {
        fprintf(file, "[%s] pid:%ld [%s] :] %s\n",
                timestamp, (long)getpid(), logger_level_name(level), text);
    }
    fclose(file);
}

void Logger::logger_log(LoggerOutputLevel level, const char *component, const char *format, va_list ap)   // Changed: va_list (by value), no ...
{
    const char *part = component ? component : "";

    if (level == LOGGER_LEVEL_DEBUG && !logger_get_debug()) {
        return;
    }

    /* Quiet hides every human console line, including ERROR, CRITICAL, and
       FATAL. The dated file still receives the line. Stdout lines stay hidden
       until --verbose or debug mode, and neither one shows them while quiet.
       JSON replaces the human line. */
    // Make a copy because g_strdup_vprintf / g_vasprintf may consume the list
    va_list ap_copy;
    va_copy(ap_copy, ap);

    gchar *message = g_strdup_vprintf(format, ap_copy);

    logger_note_dated_path();
    logger_append_log_file(level, part, message);

    if (bound->jsonMode) {
        gchar *escaped = logger_json_escape(message ? message : "");
        g_print("{\"timestamp\":%ld,\"level\":\"%s\",\"message\":\"%s\"}\n",
                (long)time(NULL),
                logger_level_name(level),
                escaped);
        g_free(escaped);
    } else if (!logger_get_quiet() &&
               (logger_stderr_level(level) || bound->verbose || bound->debugEnabled)) {
        logger_emit_chronicle(level, part, message);
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
void Logger::logger_info(const char *component, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    logger_log(LOGGER_LEVEL_INFO, component, format, args);
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
void Logger::logger_warn(const char *component, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    logger_log(LOGGER_LEVEL_WARNING, component, format, args);
    va_end(args);
}

// ──────────────────────────────────────────────
// logger_error() - Convenience Function: logger_error
// ──────────────────────────────────────────────
//
// Purpose:
//   Error messages. Quiet hides the human console line. The dated file still receives it.
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
//   The dated file still receives the line. Quiet hides the human console line.
//
// Last updated: Full CIAO expansion (2026-05-08)
// ──────────────────────────────────────────────
void Logger::logger_error(const char *component, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    logger_log(LOGGER_LEVEL_ERROR, component, format, args);
    va_end(args);
}

void Logger::logger_critical(const char *component, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    logger_log(LOGGER_LEVEL_CRITICAL, component, format, args);
    va_end(args);
}

void Logger::logger_fatal(const char *component, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    logger_log(LOGGER_LEVEL_FATAL, component, format, args);
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
//   - Only active for --debug, or for DEBUG or debug set to show, true, or 1.
//   - Quiet hides the human debug line.
//   - JSON mode still emits that call as one JSON object.
//
// Why This Exists:
//   Prevents debug spam in normal runs while allowing deep inspection when needed.
//
// Last updated: Full CIAO expansion (2026-05-08)
// ──────────────────────────────────────────────
void Logger::logger_debug(const char *component, const char *format, ...)
{
    if (!logger_get_debug()) return;

    va_list args;
    va_start(args, format);
    logger_log(LOGGER_LEVEL_DEBUG, component, format, args);
    va_end(args);
}

/* Purpose: record one loggable debug line.
   Parameters: component names the type. message is the call or the member change.
   Dependencies: logger_get_debug, logger_emit_chronicle, logger_debug.
   Why This Design: a reader command must not open a log file and must not run retention.
   JSON on a reader stays one object. Quiet hides the console line.
   A recording command uses the ordinary debug line, including the dated file.
   CIAO Principles: Caution on the reader gate. The predicate stays logger_get_debug.
   Protection Rule: requirement-cpp-debug-mode.md. No second debug predicate.
   Last updated: 2026-11-11 */
void Logger::logger_record_debug(const char *component, const char *message)
{
    const char *part = component != NULL ? component : "";
    const char *text = message != NULL ? message : "";

    if (!logger_get_debug()) {
        return;
    }
    if (bound->reader) {
        if (bound->jsonMode || logger_get_quiet()) {
            return;
        }
        logger_emit_chronicle(LOGGER_LEVEL_DEBUG, part, text);
        return;
    }
    logger_debug(part, "%s", text);
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
    Help help;

    help.bind(*this);
    help.printPage();
}


/* Purpose: C linkage for the names already declared in logger.h.
   Each logging name forwards to the object main constructed.
   logger_print_about constructs About and prints that page.
   logger_maintain_directory, with nothing bound, uses a short-lived object
   so the retain driver does not parse argv or append a log.
   Protection Rule: requirement-cpp-oop.md. These names are not new behavior. */

extern "C" void logger_log(LoggerOutputLevel level, const char *component, const char *format, va_list ap)
{
    if (Logger::logger_active == NULL) {
        g_printerr("Logger is constructed at the call site\n");
        exit(1);
    }
    Logger::logger_active->logger_log(level, component, format, ap);
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

extern "C" bool logger_get_json(void)
{
    if (Logger::logger_active == NULL) {
        g_printerr("Logger is constructed at the call site\n");
        exit(1);
    }
    return Logger::logger_active->logger_get_json();
}

extern "C" const char *logger_persistence_storage(void)
{
    if (Logger::logger_active == NULL) {
        g_printerr("Logger is constructed at the call site\n");
        exit(1);
    }
    return Logger::logger_active->logger_persistence_storage();
}

extern "C" void logger_info(const char *component, const char *format, ...)
{
    va_list ap;

    if (Logger::logger_active == NULL) {
        g_printerr("Logger is constructed at the call site\n");
        exit(1);
    }
    va_start(ap, format);
    Logger::logger_active->logger_log(LOGGER_LEVEL_INFO, component, format, ap);
    va_end(ap);
}

extern "C" void logger_warn(const char *component, const char *format, ...)
{
    va_list ap;

    if (Logger::logger_active == NULL) {
        g_printerr("Logger is constructed at the call site\n");
        exit(1);
    }
    va_start(ap, format);
    Logger::logger_active->logger_log(LOGGER_LEVEL_WARNING, component, format, ap);
    va_end(ap);
}

extern "C" void logger_error(const char *component, const char *format, ...)
{
    va_list ap;

    if (Logger::logger_active == NULL) {
        g_printerr("Logger is constructed at the call site\n");
        exit(1);
    }
    va_start(ap, format);
    Logger::logger_active->logger_log(LOGGER_LEVEL_ERROR, component, format, ap);
    va_end(ap);
}

extern "C" void logger_critical(const char *component, const char *format, ...)
{
    va_list ap;

    if (Logger::logger_active == NULL) {
        g_printerr("Logger is constructed at the call site\n");
        exit(1);
    }
    va_start(ap, format);
    Logger::logger_active->logger_log(LOGGER_LEVEL_CRITICAL, component, format, ap);
    va_end(ap);
}

extern "C" void logger_fatal(const char *component, const char *format, ...)
{
    va_list ap;

    if (Logger::logger_active == NULL) {
        g_printerr("Logger is constructed at the call site\n");
        exit(1);
    }
    va_start(ap, format);
    Logger::logger_active->logger_log(LOGGER_LEVEL_FATAL, component, format, ap);
    va_end(ap);
}

extern "C" void logger_debug(const char *component, const char *format, ...)
{
    va_list ap;

    if (Logger::logger_active == NULL) {
        g_printerr("Logger is constructed at the call site\n");
        exit(1);
    }
    va_start(ap, format);
    Logger::logger_active->logger_log(LOGGER_LEVEL_DEBUG, component, format, ap);
    va_end(ap);
}

extern "C" void logger_maintain_directory(const char *folder, const char *active_basename, const char *today_yyyymmdd)
{
    if (Logger::logger_active == NULL) {
        About coldAbout;

        coldAbout.setName("");
        Logger cold(coldAbout, NULL);
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
    AboutCli page;

    if (Logger::logger_active == NULL) {
        g_printerr("Logger is constructed at the call site\n");
        exit(1);
    }
    page.bind(*Logger::logger_active);
    page.printPage();
}
