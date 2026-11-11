// help.cpp
// Usage page. JSON is one object. The human page lists verbs and flags.
// The page binds Logger and is loggable.
// g_print in this file prints that page. It does not write a log line.

#include "help.h"
#include "logger.h"

#include <glib.h>

// Purpose:
//   Prepare an unbound page. bind() stores the program before printPage().
// Parameters:
//   None.
// Dependencies:
//   help.h
// Why This Design:
//   The page reads the name and the JSON flag from the bound logger.
// CIAO Principles Applied:
//   Intentional: one bind, then one print.
// Protection Rule:
//   requirement-cpp-parameters.md. The page lists every flag.
// Last updated: 2026-11-11
Help::Help(void)
    : bound(NULL)
{
}

// Purpose:
//   Record one loggable event through the bound logger.
// Parameters:
//   event is the public call or the major member change.
// Dependencies:
//   Logger::logger_record_debug
// Why This Design:
//   printPage records one call. Each g_print stays inside that call.
// CIAO Principles Applied:
//   Caution: an unbound page records nothing.
// Protection Rule:
//   The record uses the debug gate. This method does not open a file.
// Last updated: 2026-11-11
void Help::note(const char *event)
{
    if (bound == NULL || event == NULL) {
        return;
    }
    bound->logger_record_debug("Help", event);
}

// Purpose:
//   Bind the logger. The name and the JSON flag stay on the program it holds.
// Parameters:
//   logger is the logger for this run.
// Dependencies:
//   Logger::boundAbout
// Why This Design:
//   Help does not keep a second copy of the name or the flags.
// CIAO Principles Applied:
//   Intentional: the call and the member change are two records.
// Protection Rule:
//   The page does not parse argv and does not exit the process.
// Last updated: 2026-11-11
void Help::bind(Logger &logger)
{
    int changed = (bound != &logger);

    bound = &logger;
    note("Help::bind");
    if (changed) {
        note("Help bound");
    }
}

// Purpose:
//   Print usage. JSON prints one object and no human text.
// Parameters:
//   None. bind() has stored the program.
// Dependencies:
//   g_print, About::name, About::json, through the bound logger
// Why This Design:
//   The sentences match the flag table. The name comes from About.
// CIAO Principles Applied:
//   Caution: JSON and the human page are two outputs of one page.
// Protection Rule:
//   requirement-cpp-parameters.md. Quiet does not hide this page.
// Last updated: 2026-11-11
void Help::printPage(void)
{
    About &about = bound->boundAbout();
    const char *program;

    note("Help::printPage");
    program = about.name();
    if (about.json()) {
        g_print("{\"command\":\"help\",\"verbs\":[\"help\",\"about\",\"test\",\"list\",\"show\"],\"flags\":[\"--verbose\",\"--quiet\",\"--json\",\"--debug\",\"--prefix\",\"--home\",\"--log-datestamp\"]}\n");
        return;
    }
    g_print("Usage: %s [help|about|test|list|show] [options]\n\n", program);
    g_print("Commands:\n");
    g_print("  help                 Show this help message\n");
    g_print("  about                Show this program, this computer, and where it is running from\n");
    g_print("  test                 Write one test log line. Requires --prefix or --home, and --log-datestamp\n");
    g_print("  list                 List regular *.log names. Requires --prefix or --home\n");
    g_print("  show                 Print one log file. Requires --prefix or --home, and --log-datestamp\n");
    g_print("  One of test, list, or show. A second command stops.\n\n");
    g_print("Options:\n");
    g_print("  --verbose            Show stdout log lines. Ignored with --quiet or --json\n");
    g_print("  --quiet              Hide every human console line. The log file still receives it\n");
    g_print("  --json               JSON mode. Each verb prints one JSON object. --verbose does not add a human line\n");
    g_print("  --debug              Enable debug events and show stdout log lines. Hidden with --quiet or --json\n");
    g_print("  --prefix <dir>       Test switch for root or sudo. Log directory <dir>/var/%s/log\n", program);
    g_print("  --home <dir>         Test switch for any other user. Log directory <dir>/.app/%s/log\n", program);
    g_print("  --log-datestamp <YYYYMMDD>  Log file %s-<YYYYMMDD>.log\n\n", program);
    g_print("Environments:\n");
    g_print("  root or sudo         Base /var/%s and log directory /var/%s/log\n", program, program);
    g_print("  other                Base $HOME/.app/%s and log directory $HOME/.app/%s/log\n\n", program, program);
    g_print("Environment Variables:\n");
    g_print("  DEBUG=1              Enable debug. Also show, true, or 1. The name debug matches too\n");
    g_print("  QUIET=1              Same as --quiet\n");
    g_print("  JSON=1               Same as --json\n");
    g_print("  HOME                 Other-user base on a normal run. A test purpose command ignores it\n\n");
    g_print("Examples:\n");
    g_print("  DEBUG=1 %s\n", program);
    g_print("  %s test --home /tmp --log-datestamp 20261003 --debug\n", program);
    g_print("  %s test --prefix /tmp --log-datestamp 20261003 --debug\n", program);
    g_print("  %s list --home /tmp\n", program);
    g_print("  %s show --home /tmp --log-datestamp 20261003\n", program);
    g_print("  %s --verbose\n", program);
    g_print("  %s --quiet\n", program);
    g_print("  %s --json\n", program);
    g_print("  %s about\n", program);
}
