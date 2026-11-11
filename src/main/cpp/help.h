// help.h
// Usage page for the console and for JSON.
// The page binds Logger and is loggable.
// The name and the JSON flag stay on the About that logger holds.

#ifndef HELP_H
#define HELP_H

class Logger;

// Purpose: print usage. JSON prints one object. The human page lists the verbs and the flags.
// Debug mode records bind and printPage. A private helper does not record a second line.
class Help {
public:
    Help(void);
    void bind(Logger &logger);
    void printPage(void);

private:
    Logger *bound;

    void note(const char *event);
};

#endif // HELP_H
