// main.cpp
// Entry point. Console only. No GTK and no WebKit.

#include "logger.h"
#include "project.h"

// Purpose:
//   Parse the command, construct the logger, and report the version.
// Parameters:
//   argc, argv — parsed before construction.
// Dependencies:
//   logger_parse_invocation, Logger, project.h version macros.
// Why This Design:
//   The window, WebKit view, and userscript host are removed.
//   main parses, constructs Logger with five arguments, then logs.
// CIAO Principles Applied:
//   Caution: no toolkit init.
//   Intentional: one path from argv through the parser to Logger.
// Protection Rule:
//   Do not call gtk_init or create a WebKit view from this file.
//   Do not declare a second type in this file.

int main(int argc, char **argv)
{
    const char *logdir = NULL;
    const char *basedir = NULL;
    bool quiet = false;

    logger_parse_invocation(argc, argv, &logdir, &basedir, &quiet);
    Logger logger(PROJECT_NAME, logdir, basedir, quiet, NULL);
    logger.logger_finish_command();
    if (logger.logger_is_test_verb()) {
        logger.logger_info(LOG_COMPONENT, "test log");
        logger.logger_debug(LOG_COMPONENT, "component %s end", LOG_COMPONENT);
        return 0;
    }
    logger.logger_info(LOG_COMPONENT, "Starting %s %d.%d.%d",
              PROJECT_NAME, VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH);
    logger.logger_debug(LOG_COMPONENT, "component %s end", LOG_COMPONENT);
    logger.logger_info(LOG_COMPONENT, "Finished %s %d.%d.%d",
              PROJECT_NAME, VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH);
    return 0;
}
