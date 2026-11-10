// Format driver. Proves an empty name, kebab-case, the six-level line, and clock failure.
// Does not parse argv. Does not read HOME. Paths are literal /tmp directories.

#include "logger.h"
#include <string.h>

static time_t fail_clock(void)
{
    return (time_t)-1;
}

int main(int argc, char **argv)
{
    if (argc != 2) {
        return 2;
    }
    if (strcmp(argv[1], "empty") == 0) {
        Logger logger("", "/tmp/cpp-logger-format-empty", NULL, false, NULL);
        return 0;
    }
    if (strcmp(argv[1], "lines") == 0) {
        Logger logger("CppLogger", "/tmp/cpp-logger-format-log", NULL, false, NULL);
        logger.logger_info("", "no component");
        logger.logger_critical("database", "Connected");
        return 0;
    }
    if (strcmp(argv[1], "quiet") == 0) {
        Logger logger("CppLogger", "/tmp/cpp-logger-format-quiet", NULL, true, NULL);
        return 0;
    }
    if (strcmp(argv[1], "fail") == 0) {
        Logger logger("cpp-logger", "/tmp/cpp-logger-format-fail", NULL, false, fail_clock);
        logger.logger_info("main", "clock failed");
        return 0;
    }
    return 2;
}
