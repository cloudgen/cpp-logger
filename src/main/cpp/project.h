// project.h
// Compile-time macros for the name, the version, and the description.
// About loads these macros and is the runtime source of the name, the version, the paths, and the flags.
// The ship unit does not link GTK or WebKit.

#ifndef PROJECT_H
#define PROJECT_H

#define PROJECT_ID      "cpp-logger"
#define PROJECT_NAME    "cpp-logger"

#define VERSION_MAJOR   1
#define VERSION_MINOR   2
#define VERSION_PATCH   1

#define PROJECT_DESCRIPTION "Console logger. Lines follow ChronicleLogger."

#define LOG_COMPONENT "main"

#endif // PROJECT_H
