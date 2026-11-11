// logger.h
// =========================================================================
// CIAO DEFENSIVE CODING STYLE - OUTPUT SYSTEM HEADER (GNOME APPLICATION)
// =========================================================================
//
// Console output goes through logger_* .
// printf, fprintf, and perror stay out of product code.
// g_print and g_printerr are allowed inside logger.cpp for log lines.
// aboutcli.cpp prints the about page. help.cpp prints usage. Those pages are not log lines.
// 
// !!! DO NOT REMOVE, SHORTEN, COMMENT OUT, OR BYPASS ANY PART OF THIS HEADER !!!
// 
// Last aligned with SyncPrjs CIAO style + GNOME + JSON + DEBUG + "help": 2026-04-12
// =========================================================================

#ifndef LOGGER_H
#define LOGGER_H

#include <stdbool.h>
#include <stddef.h>
#include <stdarg.h>
#include <time.h>
#include <glib.h>

// =========================================================================
// Quiet and debug are read through functions. The flags stay inside logger.cpp.
// Stdout log lines are hidden unless --verbose or debug mode is set. --verbose and debug mode have no effect while quiet or JSON is on.
// ERROR, CRITICAL, and FATAL use stderr when a human line is shown. Quiet hides every human console line, including those three. The dated file still receives the line.
// JSON mode forces quiet, so logger_set_quiet cannot turn quiet off while JSON is on.
// Debug is captured once when main constructs Logger, from --debug or from DEBUG or debug set to show, true, or 1. There is no logger_set_debug.
// logger_persistence_storage returns the log directory. It does not create that directory.
// The about page does not print that return. Persistence storage is the application directory.
// The log folder is that directory plus /log. Class AboutCli owns that page and binds Logger.
// Two environments: root or sudo uses /var/<name>/log. Every other user uses $HOME/.app/<name>/log.
// test, list, and show use --prefix <dir> for <dir>/var/<name>/log and --home <dir> for <dir>/.app/<name>/log.
// list prints regular *.log names. show prints the file named by --log-datestamp.
// Quiet, JSON, and a missing --verbose do not hide those prints. They are not log lines.
// --log-datestamp names the file. test, list, and show ignore PREFIX and HOME.
// A debug event names both paths. Debug mode shows stdout log lines, and not while quiet or JSON.
// About owns the name, version, paths, and flags. Logger binds About.
// Invocation, AboutCli, and Help bind Logger. Each is loggable.
// Under debug mode, a public call and a major member change are one debug line each.
// A reader command records that line on the console and does not create a directory.
// The logger_* names below forward to the bound object. logger_print_about binds AboutCli to this logger.
// =========================================================================

typedef enum {
    LOGGER_LEVEL_INFO,
    LOGGER_LEVEL_WARNING,
    LOGGER_LEVEL_ERROR,
    LOGGER_LEVEL_CRITICAL,
    LOGGER_LEVEL_FATAL,
    LOGGER_LEVEL_DEBUG
} LoggerOutputLevel;

typedef time_t (*LoggerClock)(void);

#ifdef __cplusplus

#include "about.h"

// Purpose: one type owns the line, the dated file, retention, list, and show.
// It binds About for the name, the paths, and the flags.
// logger_active is the object a C caller can reach. It is not a function that returns a Logger.
class Logger {
    friend class About;

public:
    // C callers, including the retain driver, have no object parameter.
    static Logger *logger_active;

    Logger(About &about, LoggerClock clock);
    About &boundAbout(void);

    int logger_is_test_verb(void);
    void logger_finish_command(void);
    void logger_set_quiet(bool quiet);
    bool logger_get_quiet(void);
    bool logger_get_debug(void);
    bool logger_get_json(void);
    const char *logger_persistence_storage(void);
    void logger_log(LoggerOutputLevel level, const char *component, const char *format, va_list ap);
    void logger_info(const char *component, const char *format, ...);
    void logger_warn(const char *component, const char *format, ...);
    void logger_error(const char *component, const char *format, ...);
    void logger_critical(const char *component, const char *format, ...);
    void logger_fatal(const char *component, const char *format, ...);
    void logger_debug(const char *component, const char *format, ...);
    void logger_record_debug(const char *component, const char *message);
    void logger_maintain_directory(const char *folder, const char *active_basename, const char *today_yyyymmdd);
    void logger_print_help(void);

private:
    About *bound;
    char logger_folder[4096];
    char logger_log_path[4096];
    int logger_retention_done;
    int logger_maintain_busy;
    char logger_name[256];
    char logger_old_log_path[4096];
    LoggerClock logger_clock;

    gchar *logger_json_escape(const char *text);
    const char *logger_command_name(void);
    void logger_die(const char *message);
    void logger_join_base(char *out, size_t out_size, const char *head, const char *tail);
    int logger_datestamp_ok(const char *text);
    void logger_apply_base(const char *base, const char *stamp);
    void logger_set_purpose(int purpose);
    void logger_list_emit(GSList *names);
    // GCompareFunc. A non-static method would add an object parameter.
    static gint logger_filename_cmp(gconstpointer left, gconstpointer right);
    void logger_list_log_files(void);
    void logger_show_log_file(void);
    const char *logger_take_value(int argc, char **argv, int *index, const char *flag);
    void logger_log_found_switches(void);
    const char *logger_level_name(LoggerOutputLevel level);
    int logger_stderr_level(LoggerOutputLevel level);
    void logger_store_name(const char *logname);
    void logger_file_stamp(char *stamp, size_t stamp_size);
    void logger_clock_day(char *stamp, size_t stamp_size);
    void logger_format_timestamp(char *timestamp, size_t timestamp_size);
    void logger_apply_folder(const char *folder, const char *stamp);
    int logger_try_env_base(const char *env_name, const char *stamp);
    void logger_resolve_paths(const char *logdir, const char *basedir);
    void logger_ensure_log_directory(void);
    void logger_note_dated_path(void);
    void logger_emit_chronicle(LoggerOutputLevel level, const char *component, const char *message);
    void logger_unlink_regular(const char *path);
    int logger_retention_join(char *out, size_t out_size, const char *folder, const char *name);
    int logger_stamp_to_date(const char *stamp, GDate *date);
    int logger_retention_stamp(const char *name, char stamp[9]);
    int logger_spawn_ok(gchar **argv);
    int logger_compress_day(const char *folder, const char *stamp, const char *plain_name);
    void logger_maintain_one(const char *folder, const char *active_basename, const GDate *today, const char *stamp);
    void logger_maintain_body(const char *folder, const char *active_basename, const char *today_yyyymmdd);
    void logger_append_log_file(LoggerOutputLevel level, const char *component, const char *message);
};

#endif

// =========================================================================
// Core logging function - Single Source of Truth for ALL output
//
// Critical va_list Rule (Security & Correctness - CIAO enforced):
//   This function takes va_list BY VALUE (not "...").
//   The convenience wrappers (logger_info, logger_warn, etc.) do va_start()
//   and pass the va_list to this function.
//   Inside logger_log we MUST use va_copy() before consuming it with
//   g_strdup_vprintf(), because the list is advanced.
//cd 
//   NEVER change this back to "..." without also updating all wrappers.
//   This prevents the "incorrectly passing a va_list" warning / UB.
//
// Protection Rule:
//   Do NOT bypass this function. Do NOT use stdio directly.
// =========================================================================

#ifdef __cplusplus
extern "C" {
#endif

void logger_log(LoggerOutputLevel level, const char *component, const char *format, va_list ap);

// =========================================================================
// Convenience functions (Single Point of Entry)
// =========================================================================

void logger_output_init(int argc, char **argv);
int logger_is_test_verb(void);
void logger_set_quiet(bool quiet);
bool logger_get_quiet(void);
bool logger_get_debug(void);
const char *logger_persistence_storage(void);
void logger_info(const char *component, const char *format, ...);
void logger_warn(const char *component, const char *format, ...);
void logger_error(const char *component, const char *format, ...);
void logger_critical(const char *component, const char *format, ...);
void logger_fatal(const char *component, const char *format, ...);
void logger_debug(const char *component, const char *format, ...);     // Controlled by --debug or DEBUG/debug show, true, or 1

/* Purpose: apply the 7-day package and the 30-day removal to other dated names.
   Parameters: folder is the log directory. active_basename is the plain file
   this run is appending. today_yyyymmdd is the local date, eight digits.
   Dependencies: the log directory, tar on this host, zip on Windows.
   Why This Design: a check can call the same function with a frozen date.
   CIAO Principles: a failed package leaves the plain file. This function does not exit.
   Protection Rule: requirement-cpp-log-retention.md. help, about, list, and show do not call this. A log call does, including test.
   Last updated: 2026-11-10 */
void logger_maintain_directory(const char *folder, const char *active_basename, const char *today_yyyymmdd);

void logger_print_help(void);
void logger_print_about(void);

#ifdef __cplusplus
}
#endif

#endif // LOGGER_H
