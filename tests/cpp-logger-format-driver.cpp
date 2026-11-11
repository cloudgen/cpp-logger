// Format driver. Proves an empty name, kebab-case, the six-level line, and clock failure.
// Does not parse argv. Does not read HOME. Paths are literal /tmp directories.

#include "about.h"
#include "logger.h"
#include <string.h>

static time_t fail_clock(void)
{
    return (time_t)-1;
}

static void bind_named(About &about, const char *logname, const char *logdir, bool is_quiet)
{
    about.setName(logname);
    about.setLogDir(logdir);
    about.setQuiet(is_quiet);
}

int main(int argc, char **argv)
{
    if (argc != 2) {
        return 2;
    }
    if (strcmp(argv[1], "empty") == 0) {
        About about;

        about.setName("");
        Logger logger(about, NULL);
        return 0;
    }
    if (strcmp(argv[1], "lines") == 0) {
        About about;

        bind_named(about, "CppLogger", "/tmp/cpp-logger-format-log", false);
        Logger logger(about, NULL);
        logger.logger_info("", "no component");
        logger.logger_critical("database", "Connected");
        return 0;
    }
    if (strcmp(argv[1], "quiet") == 0) {
        About about;

        bind_named(about, "CppLogger", "/tmp/cpp-logger-format-quiet", true);
        Logger logger(about, NULL);
        return 0;
    }
    if (strcmp(argv[1], "errors") == 0) {
        About about;

        bind_named(about, "cpp-logger", "/tmp/cpp-logger-format-errors", true);
        Logger logger(about, NULL);
        logger.logger_error("main", "hidden error");
        logger.logger_critical("main", "hidden critical");
        logger.logger_fatal("main", "hidden fatal");
        g_print("storage:%s\n", logger.logger_persistence_storage());
        return 0;
    }
    if (strcmp(argv[1], "jsonerr") == 0) {
        About about;

        bind_named(about, "cpp-logger", "/tmp/cpp-logger-format-jsonerr", false);
        Logger logger(about, NULL);
        logger.logger_error("main", "json error");
        return 0;
    }
    if (strcmp(argv[1], "token") == 0) {
        About about;

        bind_named(about, "cpp-logger", "/tmp/cpp-logger-format-token", false);
        Logger logger(about, NULL);
        if (logger.logger_get_debug()) {
            g_print("debug-on\n");
        } else {
            g_print("debug-off\n");
        }
        logger.logger_debug("main", "token line");
        return 0;
    }
    if (strcmp(argv[1], "fail") == 0) {
        About about;

        bind_named(about, "cpp-logger", "/tmp/cpp-logger-format-fail", false);
        Logger logger(about, fail_clock);
        logger.logger_info("main", "clock failed");
        return 0;
    }
    return 2;
}
