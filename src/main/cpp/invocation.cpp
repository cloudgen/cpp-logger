// invocation.cpp
// The command of this run. It binds Logger.
// A public call and a major member change are debug records.
// g_print stays out of this file. The bound logger owns the debug line.

#include "invocation.h"
#include "logger.h"

#include <glib.h>

// Purpose:
//   Prepare an unbound command. bind() stores the logger.
// Parameters:
//   None.
// Dependencies:
//   invocation.h
// Why This Design:
//   The command is recorded only after a logger is bound.
// CIAO Principles Applied:
//   Intentional: page, purpose, and complete start unset.
// Protection Rule:
//   This type does not parse argv and does not exit the process.
// Last updated: 2026-11-11
Invocation::Invocation(void)
    : bound(NULL),
      page(0),
      purpose(0),
      complete(0)
{
}

// Purpose:
//   Record one loggable event through the bound logger.
// Parameters:
//   event is the public call or the major member change.
// Dependencies:
//   Logger::logger_record_debug
// Why This Design:
//   A private helper does not record a second line.
// CIAO Principles Applied:
//   Caution: an unbound object records nothing.
// Protection Rule:
//   The record uses the debug gate. This method does not open a file.
// Last updated: 2026-11-11
void Invocation::note(const char *event)
{
    if (bound == NULL || event == NULL) {
        return;
    }
    bound->logger_record_debug("Invocation", event);
}

// Purpose:
//   Bind the logger that records this command.
// Parameters:
//   logger is the logger for this run.
// Dependencies:
//   Logger::logger_record_debug
// Why This Design:
//   The pointer is the bound logger. The command does not copy the flags.
// CIAO Principles Applied:
//   Intentional: the call and the member change are two records.
// Protection Rule:
//   About stays the source of the name, the version, the paths, and the flags.
// Last updated: 2026-11-11
void Invocation::bind(Logger &logger)
{
    int changed = (bound != &logger);

    bound = &logger;
    note("Invocation::bind");
    if (changed) {
        note("Invocation bound");
    }
}

// Purpose:
//   Copy the page and the purpose from the program the logger holds.
// Parameters:
//   None. bind() has stored the logger.
// Dependencies:
//   About::pageValue, About::purposeValue
// Why This Design:
//   A value that did not change is not a major member change.
// CIAO Principles Applied:
//   Caution: the call is recorded even when both values stay.
// Protection Rule:
//   This method does not choose the verb.
// Last updated: 2026-11-11
void Invocation::takeCommand(void)
{
    About &about = bound->boundAbout();
    int nextPage = about.pageValue();
    int nextPurpose = about.purposeValue();
    char line[64];

    note("Invocation::takeCommand");
    if (page != nextPage) {
        page = nextPage;
        g_snprintf(line, sizeof line, "Invocation page %d", page);
        note(line);
    }
    if (purpose != nextPurpose) {
        purpose = nextPurpose;
        g_snprintf(line, sizeof line, "Invocation purpose %d", purpose);
        note(line);
    }
}

// Purpose:
//   Mark the command finished after the verb has run.
// Parameters:
//   None.
// Dependencies:
//   Logger::logger_record_debug
// Why This Design:
//   complete moves from unset to finished once.
// CIAO Principles Applied:
//   Intentional: a second mark records the call and not a second change.
// Protection Rule:
//   This method does not exit the process.
// Last updated: 2026-11-11
void Invocation::markComplete(void)
{
    note("Invocation::markComplete");
    if (complete == 0) {
        complete = 1;
        note("Invocation complete");
    }
}
