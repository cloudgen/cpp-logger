// aboutcli.cpp
// About page: identity, host check, install location.
// JSON and console text. The page binds Logger and is loggable.
// Persistence storage is the application directory (.app/<name> or /var/<name>).
// The log folder is that directory plus /log.
// g_print and g_printerr in this file print the page and a path error.
// They do not write a log line. They do not create a directory.

#include "aboutcli.h"
#include "logger.h"

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/utsname.h>

// Purpose:
//   Prepare an unbound page. bind() stores the program before printPage().
// Parameters:
//   None.
// Dependencies:
//   aboutcli.h
// Why This Design:
//   The page reads identity and the JSON flag from the bound logger.
// CIAO Principles Applied:
//   Intentional: one bind, then one print.
// Protection Rule:
//   requirement-cpp-about.md. Quiet does not hide the human page.
// Last updated: 2026-11-11
AboutCli::AboutCli(void)
    : bound(NULL)
{
}

// Purpose:
//   Return the program the bound logger holds.
// Parameters:
//   None.
// Dependencies:
//   Logger::boundAbout
// Why This Design:
//   The page does not keep a second copy of the name, version, or flags.
// CIAO Principles Applied:
//   Intentional: one read path for those facts.
// Protection Rule:
//   This helper does not record a second debug line.
// Last updated: 2026-11-11
About &AboutCli::identity(void)
{
    return bound->boundAbout();
}

// Purpose:
//   Record one loggable event through the bound logger.
// Parameters:
//   event is the public call or the major member change.
// Dependencies:
//   Logger::logger_record_debug
// Why This Design:
//   printPage records one call. The host reads stay inside that call.
// CIAO Principles Applied:
//   Caution: an unbound page records nothing.
// Protection Rule:
//   The record uses the debug gate. This method does not open a file.
// Last updated: 2026-11-11
void AboutCli::note(const char *event)
{
    if (bound == NULL || event == NULL) {
        return;
    }
    bound->logger_record_debug("AboutCli", event);
}

// Purpose:
//   Bind the logger. Name, version, description, and JSON stay on the program it holds.
// Parameters:
//   logger is the logger for this run.
// Dependencies:
//   Logger::boundAbout
// Why This Design:
//   This page does not keep a second copy of those facts.
// CIAO Principles Applied:
//   Intentional: the call and the member change are two records.
// Protection Rule:
//   The page does not parse argv and does not apply --home or --prefix.
// Last updated: 2026-11-11
void AboutCli::bind(Logger &logger)
{
    int changed = (bound != &logger);

    bound = &logger;
    note("AboutCli::bind");
    if (changed) {
        note("AboutCli bound");
    }
}

void AboutCli::copyText(char *out, size_t outSize, const char *value)
{
    g_strlcpy(out, value != NULL ? value : "", outSize);
}

void AboutCli::probePath(char *out, size_t outSize, const char *path, const char *version)
{
    struct stat st;

    if (path != NULL && path[0] != '\0' && stat(path, &st) == 0 &&
        S_ISREG(st.st_mode) && access(path, X_OK) == 0) {
        copyText(out, outSize, version);
        return;
    }
    copyText(out, outSize, "not found");
}

void AboutCli::joinFolder(char *out, size_t outSize, const char *head, const char *tail)
{
    gchar *trimmed;
    gsize length;
    gint wrote;

    if (head == NULL || head[0] == '\0') {
        out[0] = '\0';
        return;
    }
    trimmed = g_strdup(head);
    length = strlen(trimmed);
    while (length > 1 && trimmed[length - 1] == '/') {
        trimmed[--length] = '\0';
    }
    if (strcmp(trimmed, "/") == 0) {
        wrote = g_snprintf(out, outSize, "/%s", tail);
    } else {
        wrote = g_snprintf(out, outSize, "%s/%s", trimmed, tail);
    }
    g_free(trimmed);
    if (wrote < 0 || (size_t)wrote >= outSize) {
        g_printerr("persistence folder path is too long\n");
        exit(1);
    }
}

// Purpose:
//   Resolve the application directory. The page prints it as Persistence storage.
// Parameters:
//   out receives the directory. An unresolved directory is empty.
// Dependencies:
//   CONDA_PREFIX, PYENV_VIRTUAL_ENV, VIRTUAL_ENV, HOME, and the effective user id.
// Why This Design:
//   The first match wins. This read does not create the directory.
// CIAO Principles Applied:
//   Caution: a missing home leaves the directory empty.
// Protection Rule:
//   requirement-cpp-about.md. This page resolves the application directory itself.
// Last updated: 2026-11-11
void AboutCli::persistenceFolder(char *out, size_t outSize)
{
    const char *envNames[] = {"CONDA_PREFIX", "PYENV_VIRTUAL_ENV", "VIRTUAL_ENV", NULL};
    const char *program = identity().name();
    char tail[256];
    const char *env;
    int i;

    out[0] = '\0';
    if (g_snprintf(tail, sizeof tail, ".app/%s", program) >= (gint)sizeof tail) {
        g_printerr("persistence folder path is too long\n");
        exit(1);
    }
    for (i = 0; envNames[i] != NULL; i++) {
        env = getenv(envNames[i]);
        if (env != NULL && env[0] != '\0') {
            joinFolder(out, outSize, env, tail);
            return;
        }
    }
    if (geteuid() == 0) {
        if (g_snprintf(out, outSize, "/var/%s", program) >= (gint)outSize) {
            g_printerr("persistence folder path is too long\n");
            exit(1);
        }
        return;
    }
    env = getenv("HOME");
    if (env != NULL && env[0] != '\0') {
        joinFolder(out, outSize, env, tail);
    }
}

// Purpose:
//   The log folder is the application directory plus /log.
// Parameters:
//   persistence is the application directory. An empty directory leaves out empty.
// Dependencies:
//   persistenceFolder.
// Why This Design:
//   The two lines stay distinct. This read does not create the directory.
// CIAO Principles Applied:
//   Caution: an empty persistence directory does not invent a log folder.
// Protection Rule:
//   requirement-cpp-about.md. The page does not print the logger directory here.
// Last updated: 2026-11-11
void AboutCli::logFolder(char *out, size_t outSize, const char *persistence)
{
    if (persistence == NULL || persistence[0] == '\0') {
        out[0] = '\0';
        return;
    }
    if (g_snprintf(out, outSize, "%s/log", persistence) >= (gint)outSize) {
        g_printerr("log folder path is too long\n");
        exit(1);
    }
}

gchar *AboutCli::escapeJson(const char *text)
{
    GString *out = g_string_new("");
    const unsigned char *cursor = (const unsigned char *)(text != NULL ? text : "");

    for (; *cursor != '\0'; cursor++) {
        switch (*cursor) {
            case '\\':
                g_string_append(out, "\\\\");
                break;
            case '"':
                g_string_append(out, "\\\"");
                break;
            case '\n':
                g_string_append(out, "\\n");
                break;
            case '\r':
                g_string_append(out, "\\r");
                break;
            case '\t':
                g_string_append(out, "\\t");
                break;
            default:
                if (*cursor < 0x20) {
                    g_string_append_printf(out, "\\u%04x", *cursor);
                } else {
                    g_string_append_c(out, (gchar)*cursor);
                }
                break;
        }
    }
    return g_string_free(out, FALSE);
}

void AboutCli::appendJsonString(GString *out, int *first, const char *key, const char *value)
{
    gchar *escaped = escapeJson(value != NULL ? value : "");

    if (!*first) {
        g_string_append_c(out, ',');
    }
    *first = 0;
    g_string_append_printf(out, "\"%s\":\"%s\"", key, escaped);
    g_free(escaped);
}

// Purpose:
//   Print the three blocks, or one JSON object when About says JSON is on.
// Parameters:
//   None. bind() has stored the program.
// Dependencies:
//   uname, readlink, getenv, stat, access. No spawned compiler and no install.
// Why This Design:
//   Identity, the host check, and the install location are one page.
// CIAO Principles Applied:
//   Caution: a missing home leaves the home paths empty.
// Protection Rule:
//   requirement-cpp-about.md. The page does not create a directory.
// Last updated: 2026-11-11
void AboutCli::printPage(void)
{
    const char *program;

    note("AboutCli::printPage");
    program = identity().name();
    char version[64];
    char stamp[64];
    char user[256];
    char shell[256];
    char location[4096];
    char preferred[4096];
    char fallback1[4096];
    char used[4096];
    char persistence[4096];
    char logs[4096];
    char globalPath[4096];
    char localPath[4096];
    char globalValue[64];
    char localValue[64];
    char compiler[256];
    char osLine[256];
    char osName[128];
    char arch[128];
    const char *home;
    const char *prefix;
    time_t now;
    struct tm *tmPtr;
    struct utsname uts;
    ssize_t linkLength;
    int insideDocker;
    int tty;
    GString *out;
    int first;

    g_snprintf(version, sizeof version, "%d.%d.%d",
               identity().versionMajor(), identity().versionMinor(), identity().versionPatch());
    now = time(NULL);
    tmPtr = now == (time_t)-1 ? NULL : localtime(&now);
    if (tmPtr == NULL || strftime(stamp, sizeof stamp, "%Y-%m-%d %H:%M:%S", tmPtr) == 0) {
        g_strlcpy(stamp, "1970-01-01 00:00:00", sizeof stamp);
    }
    g_strlcat(stamp, ".000000", sizeof stamp);

    copyText(user, sizeof user, getenv("USER"));
    copyText(shell, sizeof shell, getenv("SHELL"));
    linkLength = readlink("/proc/self/exe", location, sizeof location - 1);
    if (linkLength < 0) {
        copyText(location, sizeof location, program);
    } else {
        location[linkLength] = '\0';
    }
    if (uname(&uts) == 0) {
        g_snprintf(osLine, sizeof osLine, "%s %s", uts.sysname, uts.release);
        copyText(osName, sizeof osName, uts.sysname);
        copyText(arch, sizeof arch, uts.machine);
    } else {
        osLine[0] = '\0';
        osName[0] = '\0';
        arch[0] = '\0';
    }
    insideDocker = access("/.dockerenv", F_OK) == 0;
    tty = isatty(STDOUT_FILENO) ? 1 : 0;
    home = getenv("HOME");
    preferred[0] = '\0';
    fallback1[0] = '\0';
    used[0] = '\0';
    if (geteuid() == 0) {
        g_snprintf(preferred, sizeof preferred, "/var/cache/%s", program);
        copyText(used, sizeof used, preferred);
    } else {
        g_snprintf(preferred, sizeof preferred, "/dev/shm/cache/cache-%s", program);
        if (home != NULL && home[0] != '\0') {
            g_snprintf(fallback1, sizeof fallback1, "%s/.cache/cache-%s", home, program);
        }
        if (g_file_test("/dev/shm", G_FILE_TEST_IS_DIR)) {
            copyText(used, sizeof used, preferred);
        } else {
            copyText(used, sizeof used, fallback1);
        }
    }
    persistenceFolder(persistence, sizeof persistence);
    logFolder(logs, sizeof logs, persistence);

    prefix = getenv("PREFIX");
    if (prefix != NULL && prefix[0] != '\0') {
        g_snprintf(globalPath, sizeof globalPath, "%s/bin/%s", prefix, program);
    } else {
        g_snprintf(globalPath, sizeof globalPath, "/usr/local/bin/%s", program);
    }
    probePath(globalValue, sizeof globalValue, globalPath, version);
    localPath[0] = '\0';
    if (home != NULL && home[0] != '\0') {
        g_snprintf(localPath, sizeof localPath, "%s/.local/bin/%s", home, program);
        probePath(localValue, sizeof localValue, localPath, version);
    } else {
        copyText(localValue, sizeof localValue, "not found");
    }

    if (identity().json()) {
        g_snprintf(compiler, sizeof compiler, "g++ %s", __VERSION__);
        out = g_string_new("{");
        first = 1;
        appendJsonString(out, &first, "command", "about");
        appendJsonString(out, &first, "name", program);
        appendJsonString(out, &first, "version", version);
        appendJsonString(out, &first, "description", identity().description());
        appendJsonString(out, &first, "domain", "No database. No configuration file.");
        appendJsonString(out, &first, "runtime_tools", "none");
        appendJsonString(out, &first, "entry_points", program);
        g_string_append(out, ",\"host\":{");
        first = 1;
        appendJsonString(out, &first, "stamp", stamp);
        appendJsonString(out, &first, "compiler", compiler);
        appendJsonString(out, &first, "cpp_standard", "C++17");
        appendJsonString(out, &first, "operation_system", osName);
        appendJsonString(out, &first, "architecture", arch);
        appendJsonString(out, &first, "current_user", user);
        appendJsonString(out, &first, "shell", shell);
        g_string_append_printf(out, ",\"inside_docker\":%s", insideDocker ? "true" : "false");
        appendJsonString(out, &first, "location", location);
        g_string_append_printf(out, ",\"pid\":%ld", (long)getpid());
        appendJsonString(out, &first, "cache_folder_used", used);
        appendJsonString(out, &first, "cache_folder_preferred", preferred);
        appendJsonString(out, &first, "cache_folder_1st_fallback", fallback1);
        appendJsonString(out, &first, "cache_folder_2nd_fallback", "");
        appendJsonString(out, &first, "persistence_storage", persistence);
        appendJsonString(out, &first, "log_folder", logs);
        g_string_append_printf(out, ",\"tty\":%s", tty ? "true" : "false");
        g_string_append(out, "},\"install\":{");
        first = 1;
        appendJsonString(out, &first, "global_path", globalPath);
        appendJsonString(out, &first, "global", globalValue);
        appendJsonString(out, &first, "local_path", localPath);
        appendJsonString(out, &first, "local", localValue);
        appendJsonString(out, &first, "running_from", location);
        g_string_append(out, "}}\n");
        g_print("%s", out->str);
        g_string_free(out, TRUE);
        return;
    }

    g_print("%s %s\n", program, version);
    g_print("%s\n", identity().description());
    g_print("Domain: No database. No configuration file.\n");
    g_print("Runtime tools: none\n");
    g_print("Entry points: %s\n", program);
    g_print("\n");
    g_print("%s %s(v%s)  [CHECK SYSTEM]:\n", stamp, program, version);
    g_print("  Now checking your operation system!\n");
    g_print("    Compiler: g++ %s\n", __VERSION__);
    g_print("    C++ standard: C++17\n");
    g_print("    Operation System: %s\n", osLine);
    g_print("    Architecture: %s\n", arch);
    g_print("    Current User: %s\n", user);
    g_print("    Shell: %s\n", shell);
    g_print("    Inside docker container: %s\n", insideDocker ? "True" : "False");
    g_print("    Location: %s\n", location);
    g_print("    PID: %ld\n", (long)getpid());
    g_print("    Cache folder used: %s\n", used);
    g_print("    Cache folder (preferred): %s\n", preferred);
    g_print("    Cache folder (1st fallback): %s\n", fallback1);
    g_print("    Cache folder (2nd fallback): %s\n", "");
    g_print("    Persistence storage: %s\n", persistence);
    g_print("    Log folder: %s\n", logs);
    g_print("    TTY / Interactive: %s\n", tty ? "yes" : "no");
    g_print("\n");
    g_print("Install location\n");
    g_print("    Global install (%s): %s\n", globalPath, globalValue);
    if (localPath[0] != '\0') {
        g_print("    Local install (%s): %s\n", localPath, localValue);
    } else {
        g_print("    Local install: %s\n", localValue);
    }
    g_print("    Running from: %s\n", location);
}
