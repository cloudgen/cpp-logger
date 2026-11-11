// aboutcli.h
// JSON and console text for the about page.
// The page binds Logger and is loggable.
// Name, version, description, and JSON stay on the About that logger holds.
// Persistence storage is the application directory.
// The log folder is that directory plus /log.
// This page does not create either directory.

#ifndef ABOUTCLI_H
#define ABOUTCLI_H

#include <glib.h>
#include <stddef.h>

class About;
class Logger;

// Purpose: print the three about blocks for this run.
// The page reads the environment for host facts. It does not install.
// Debug mode records bind and printPage. The page text is not a log line.
class AboutCli {
public:
    AboutCli(void);
    void bind(Logger &logger);
    void printPage(void);

private:
    Logger *bound;

    About &identity(void);
    void note(const char *event);

    void copyText(char *out, size_t outSize, const char *value);
    void probePath(char *out, size_t outSize, const char *path, const char *version);
    void joinFolder(char *out, size_t outSize, const char *head, const char *tail);
    void persistenceFolder(char *out, size_t outSize);
    void logFolder(char *out, size_t outSize, const char *persistence);
    gchar *escapeJson(const char *text);
    void appendJsonString(GString *out, int *first, const char *key, const char *value);
};

#endif // ABOUTCLI_H
