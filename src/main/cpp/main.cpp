// main.cpp
// Entry point. Console only. No GTK and no WebKit.

#include "logger.h"
#include "project.h"

// Purpose:
//   Start the output system and report the version.
// Parameters:
//   argc, argv — passed to logger_output_init.
// Dependencies:
//   logger_output_init, logger_info, project.h version macros.
// Why This Design:
//   The window, WebKit view, and userscript host are removed.
//   main only starts logging.
// CIAO Principles Applied:
//   Caution: no toolkit init.
//   Intentional: one path from argv to logger_*.
// Protection Rule:
//   Do not call gtk_init or create a WebKit view from this file.

int main(int argc, char **argv)
{
    logger_output_init(argc, argv);
    if (logger_is_test_verb()) {
        logger_info("test log");
        logger_debug("component %s end", LOG_COMPONENT);
        return 0;
    }
    logger_info("Starting %s %d.%d.%d",
              PROJECT_NAME, VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH);
    logger_debug("component %s end", LOG_COMPONENT);
    logger_info("Finished %s %d.%d.%d",
              PROJECT_NAME, VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH);
    return 0;
}
