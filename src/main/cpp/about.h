// about.h
// Runtime source of name, version, paths, and flags.
// Logger binds this object. Invocation, AboutCli, and Help bind Logger.
// project.h holds the compile-time macros this object loads.

#ifndef ABOUT_H
#define ABOUT_H

#include <stddef.h>

// Purpose: one object owns this run's name, version, paths, and flags.
// The entry constructs it. It parses, binds the logger, routes one verb, and finishes.
class About {
    friend class Logger;

public:
    About(void);

    void setName(const char *name);
    void setLogDir(const char *dir);
    void setBaseDir(const char *dir);
    void setQuiet(bool quiet);
    void parse(int argc, char **argv);
    int run(int argc, char **argv);

    const char *name(void) const;
    const char *description(void) const;
    const char *component(void) const;
    int versionMajor(void) const;
    int versionMinor(void) const;
    int versionPatch(void) const;
    const char *logDir(void) const;
    const char *baseDir(void) const;
    bool quiet(void) const;
    bool json(void) const;
    bool debug(void) const;
    int verboseValue(void) const;
    int purposeValue(void) const;
    int pageValue(void) const;
    int readerValue(void) const;
    const char *prefixValue(void) const;
    const char *homeValue(void) const;
    const char *datestampValue(void) const;

private:
    void captureEnvironment(void);
    void resetFlags(void);
    void die(const char *message);
    const char *commandName(void) const;
    const char *takeValue(int argc, char **argv, int *index, const char *flag);
    void setPurpose(int next);
    void setPage(int next);
    int datestampOk(const char *text) const;
    void joinBase(char *out, size_t outSize, const char *head, const char *tail);

    char nameStorage[256];
    char descriptionStorage[512];
    char componentStorage[64];
    char baseStorage[4096];
    int versionMajorValue;
    int versionMinorValue;
    int versionPatchValue;
    const char *logDirValue;
    const char *baseDirValue;
    bool quietMode;
    bool jsonMode;
    bool debugEnabled;
    int verbose;
    int purpose;
    const char *prefix;
    const char *home;
    const char *datestamp;
    int sawQuiet;
    int sawJson;
    int sawDebug;
    int sawVerbose;
    int page;
    int reader;
};

#endif // ABOUT_H
