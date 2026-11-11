// invocation.h
// The command of this run. It binds Logger and is loggable.
// About remains the source of the name, the version, the paths, and the flags.

#ifndef INVOCATION_H
#define INVOCATION_H

class Logger;

// Purpose: record which command this run is, and whether that command finished.
// Debug mode records each public call and each major member change.
class Invocation {
public:
    Invocation(void);
    void bind(Logger &logger);
    void takeCommand(void);
    void markComplete(void);

private:
    Logger *bound;
    int page;
    int purpose;
    int complete;

    void note(const char *event);
};

#endif // INVOCATION_H
