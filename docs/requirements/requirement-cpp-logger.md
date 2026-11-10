**file**: docs/requirements/requirement-cpp-logger.md
**Status**: Active
**Updated**: 2026-11-11
**Philosophy**: CIAO v2.10.2 / CIAO-Lite (Caution • Intentional • Anti-fragile • Over-engineered / Over-protect)

## 1. Purpose

This file owns the logger calls, the human line, and the dated file that stores that line. `requirement-cpp-logger-folder.md` owns the directory. `requirement-cpp-output.md` owns which stream the line uses and what quiet hides. `requirement-cpp-json.md` owns the object that replaces the line on the terminal.

### 1.1 Human-facing

**In one sentence:** The logger is constructed with a name, two optional directories, quiet, and a clock, and each message uses `[YYYY-MM-DD HH:MM:SS] pid:<pid> [LEVEL]` plus an optional ` @component`, then ` :] message`.

| Box | Meaning | Example |
|-----|---------|---------|
| You / this login | The person watching the terminal | You start the program with no flags |
| The other role | `logger.cpp` | It formats the line and appends that line to the dated file |
| Not this file | Quiet, streams, and the JSON object | `requirement-cpp-output.md` and `requirement-cpp-json.md` |

| Includes | Excludes |
|----------|----------|
| Construction of the logger, the six level names, the human line, and the file `<name>-<YYYYMMDD>.log` | Flag spellings, quiet's effect on stdout, and the JSON object |

| Surface | What you open | What for |
|---------|---------------|----------|
| The terminal | The human line | Read one event |
| `src/main/cpp/logger.h` | Declarations | The `logger_*` calls |
| The log file | `cpp-logger-<YYYYMMDD>.log` | Verify the same line on disk |
| `--log-datestamp` | Eight digits | Choose the date in the file name |

| You do… | What it means | What you type |
|---------|---------------|---------------|
| Start the program | Construction resolves the directories, creates the log directory, and writes one newline when the file can be appended | The binary, with no flags |
| Show the line | Info, warning, and debug use the shape below. Error, critical, and fatal do not need this switch | `--verbose` or `--debug` |

## 2. Core rules

- Product code calls `logger_info`, `logger_warn`, `logger_error`, `logger_critical`, `logger_fatal`, and `logger_debug`. Those six call `logger_log`.
- The level argument is the enumeration `LoggerOutputLevel` in `logger.h`. The enumeration names are `LOGGER_LEVEL_INFO`, `LOGGER_LEVEL_WARNING`, `LOGGER_LEVEL_ERROR`, `LOGGER_LEVEL_CRITICAL`, `LOGGER_LEVEL_FATAL`, and `LOGGER_LEVEL_DEBUG`. The text written in the line is `INFO`, `WARNING`, `ERROR`, `CRITICAL`, `FATAL`, and `DEBUG`. A caller does not pass a free string for the level.

### 2.0 Initialization

The logger object is constructed with five arguments: the log name, an optional log directory, an optional base directory, a quiet flag, and an optional clock. This is the same construction order as ChronicleLogger `__init__(logname, logdir, basedir, is_quiet, time_provider)`.

1. An empty log name returns immediately. Construction does not create a directory and does not write a file.
2. Store the name. A name that contains an internal capital letter is rewritten to kebab-case. `cpp-logger` has no internal capital and stays `cpp-logger`.
3. Resolve the base directory. A non-empty base directory is used as given. Otherwise the first match wins: the active conda prefix plus `/.app/<name>`, else the active pyenv virtualenv plus `/.app/<name>`, else `VIRTUAL_ENV` plus `/.app/<name>`, else effective user id 0 uses `/var/<name>`, else `$HOME/.app/<name>`. The directory table is `requirement-cpp-logger-folder.md`.
4. Resolve the log directory. A non-empty log directory is used as given. Otherwise it is the base directory plus `/log`.
5. The current file is `<log-directory>/<name>-<YYYYMMDD>.log`. The date comes from the clock argument. A null clock uses local time. Clock failure uses `19700101`.
6. Create the log directory when it is missing. A successful create prints `Created directory: <path>` unless quiet is set.
7. When that file can be opened for append, write one newline and close the file. A permission failure does not abort construction. This newline is not a formatted log line.
8. `main` parses `argc` and `argv`. That parse is not this constructor. `main` then constructs `Logger` with the five arguments. `--prefix` and `--home` on `test`, `list`, and `show` supply an explicit directory and stay `requirement-cpp-logger-folder.md`.

The 1.1.3 source constructs `Logger` with the log name, the log directory, the base directory, quiet, and the clock. An empty name returns before a directory is created. Construction does not emit `cpp-logger Output System initialized`. `main` calls `logger_parse_invocation` and then that constructor.
- The translation unit is `src/main/cpp/logger.cpp`. The header is `src/main/cpp/logger.h`. There is no `logger.c`, no `output.c`, and no `output.h`.
- `logger_log` takes `va_list` by value. It uses `va_copy` before `g_strdup_vprintf`. The signature does not go back to `...`.
- `logger_debug` is enabled by `--debug`, or by `DEBUG` or `debug` set to `show`, `true`, or `1`. That predicate is `requirement-cpp-debug-mode.md`. There is no `logger_set_debug`.
- `logger_persistence_storage()` returns the log directory from steps 3 and 4, or an empty string when that directory is unresolved. The read does not create the directory. The about page prints that return.

### 2.1 Logger format

The line is the ChronicleLogger `log_message` line. Local time, the process id, the level, the optional component, then the message. One line, one trailing newline. The writer does not hard-wire the component. The caller passes it. This product's own events pass `main` (`LOG_COMPONENT` in `project.h`). An empty component omits the `@` slot.

```text
[YYYY-MM-DD HH:MM:SS] pid:<pid> [<LEVEL>]<component> :] <message>
```

| Field | Rule |
|-------|------|
| `YYYY-MM-DD HH:MM:SS` | Local civil time. Each number is zero-padded. The separator between date and time is one space. Clock failure uses `1970-01-01 00:00:00`. |
| `<pid>` | Decimal process id, no padding and no sign. |
| `<LEVEL>` | One of `INFO`, `WARNING`, `ERROR`, `CRITICAL`, `FATAL`, `DEBUG`, inside one pair of brackets. `logger_warn` emits `WARNING`, not `WARN`. `logger_critical` emits `CRITICAL`. `logger_fatal` emits `FATAL`. There is no omitted level. The default level, when a call does not name one, is `INFO`. |
| `<component>` | When the caller passes a non-empty component, the field is one space, `@`, and that name, with no space before `:]`. When the component is empty, the field is empty. The writer does not insert `main` on its own. |
| ` :] ` | The four characters space, colon, close-bracket, space. They stay when `<component>` is empty. |
| `<message>` | The formatted text only. No extra brackets and no second timestamp. |

```text
[2025-12-25 14:30:45] pid:12345 [INFO] @database :] Connected successfully
[2025-12-25 14:30:45] pid:12345 [INFO] :] no component
[2025-12-25 14:30:45] pid:12345 [WARNING] @main :] disk almost full
[2025-12-25 14:30:45] pid:12345 [ERROR] @main :] permission denied
[2025-12-25 14:30:45] pid:12345 [CRITICAL] @main :] cannot continue safely
[2025-12-25 14:30:45] pid:12345 [FATAL] @main :] cannot continue
[2025-12-25 14:30:45] pid:12345 [DEBUG] @main :] debug mode enabled
```

When the dated path changes inside a log call, the call runs the retention maintenance in `requirement-cpp-log-retention.md` before appending. When debug mode is on, that same call places this line ahead of the caller's line: `[YYYY-MM-DD HH:MM:SS] pid:<pid> [INFO] @logger :] Using <path>`. `ERROR`, `CRITICAL`, and `FATAL` use stderr. `INFO`, `WARNING`, and `DEBUG` use stdout. Which of those stdout lines the terminal shows is `requirement-cpp-output.md`.

Quiet and JSON do not change this shape. They choose whether this line is the one on the terminal. That choice is `requirement-cpp-output.md` and `requirement-cpp-json.md`.

### 2.1.1 Run boundary

A normal run appends `Starting cpp-logger <major>.<minor>.<patch>` and then `Finished cpp-logger <major>.<minor>.<patch>`. The numbers are `VERSION_MAJOR`, `VERSION_MINOR`, and `VERSION_PATCH` in `src/main/cpp/project.h`. Both lines are `INFO`. `help`, `about`, `list`, `show`, and `test` do not emit that pair.

When debug mode is on, the same runs that reach the logger also append `component main start` and `component main end` as `DEBUG`, and one `DEBUG` line for each switch that was present. Those lines are `requirement-cpp-debug-mode.md`. Construction does not emit `cpp-logger Output System initialized`. The construction write is the single newline in §2.0.

### 2.2 Dated file

The file is `cpp-logger-<YYYYMMDD>.log` inside cpp-log-path, or inside cpp-test-log-path when the command is `test`, `list`, or `show`. Those directories are `requirement-cpp-logger-folder.md`. The check that reads the file is `requirement-cpp-tests.md`.

| Environment or switch | File for `20261003` |
|-----------------------|---------------------|
| Root or sudo | `/var/cpp-logger/log/cpp-logger-20261003.log` |
| Other | `$HOME/.app/cpp-logger/log/cpp-logger-20261003.log` |
| `test --prefix /tmp` | `/tmp/var/cpp-logger/log/cpp-logger-20261003.log` |
| `test --home /tmp` | `/tmp/.app/cpp-logger/log/cpp-logger-20261003.log` |

1. The file name is `cpp-logger-<YYYYMMDD>.log`. It sits in cpp-log-path on a normal run and in cpp-test-log-path on `test`, `list`, and `show`. `--home /tmp` and `--prefix /tmp` do not share a file.
2. `--log-datestamp` is eight digits. Any other shape is rejected with `--log-datestamp requires YYYYMMDD`. When the switch is absent on a normal run, the stamp is today's local date, `YYYYMMDD`. Clock failure uses `19700101`.
3. The `test` command and the `show` command require the switch. A missing switch is rejected with `test purpose command requires --log-datestamp YYYYMMDD`. They do not substitute today's date. `list` does not require it.
4. Each event that `logger_log` emits is appended to this file as the human line in §2.1. The console line is unchanged. The program creates the log directory when it appends. It does not use `mkstemp`. `list` and `show` do not append a line.
5. When the log directory is unresolved, no file is created.
6. `list` prints the basenames of regular files whose names end in `.log`, one per line, in sorted order. A directory or other non-regular name is omitted. A missing directory prints no names and exits 0. It does not create the directory. A directory that exists but cannot be opened exits 1. `list` and `show` do not change the file.
7. `show` prints the contents of `cpp-logger-<YYYYMMDD>.log` for the given datestamp. A missing file exits 1. `list` and `show` use the same directory switches as `test`. `--prefix` wins when both directory switches are present. They do not read `PREFIX`, `HOME`, or the effective user id. Quiet and a missing `--verbose` do not hide these prints. JSON mode replaces them with the object in `requirement-cpp-json.md`. The display gate for log lines is `requirement-cpp-output.md`.
8. The life of a dated file is `requirement-cpp-log-retention.md`. This file owns the name and the line. It does not archive a day and it does not delete a day.

### Sample code (this project)

The blocks below are the 1.1.3 source. The caller passes the component. The enumeration includes `LOGGER_LEVEL_CRITICAL`. Construction is the five-argument form. Quiet, streams, and `--verbose` are `requirement-cpp-output.md`. The JSON object is `requirement-cpp-json.md`. The directories are `requirement-cpp-logger-folder.md`.

```cpp
const char *Logger::logger_level_name(LoggerOutputLevel level)
{
    switch (level) {
        case LOGGER_LEVEL_INFO:     return "INFO";
        case LOGGER_LEVEL_WARNING:  return "WARNING";
        case LOGGER_LEVEL_ERROR:    return "ERROR";
        case LOGGER_LEVEL_CRITICAL: return "CRITICAL";
        case LOGGER_LEVEL_FATAL:    return "FATAL";
        case LOGGER_LEVEL_DEBUG:    return "DEBUG";
        default:                    return "INFO";
    }
}

void Logger::logger_emit_chronicle(LoggerOutputLevel level, const char *component, const char *message)
{
    char timestamp[32];
    const char *text = message ? message : "";
    const char *part = component ? component : "";
    const char *level_name = logger_level_name(level);

    logger_format_timestamp(timestamp, sizeof timestamp);
    if (logger_stderr_level(level)) {
        if (part[0] != '\0') {
            g_printerr("[%s] pid:%ld [%s] @%s :] %s\n",
                       timestamp, (long)getpid(), level_name, part, text);
        } else {
            g_printerr("[%s] pid:%ld [%s] :] %s\n",
                       timestamp, (long)getpid(), level_name, text);
        }
    } else if (part[0] != '\0') {
        g_print("[%s] pid:%ld [%s] @%s :] %s\n",
                timestamp, (long)getpid(), level_name, part, text);
    } else {
        g_print("[%s] pid:%ld [%s] :] %s\n",
                timestamp, (long)getpid(), level_name, text);
    }
}

void Logger::logger_append_log_file(LoggerOutputLevel level, const char *component, const char *message)
{
    char timestamp[32];
    FILE *file;
    const char *text = message ? message : "";
    const char *part = component ? component : "";

    if (logger_log_path[0] == '\0' || logger_folder[0] == '\0') {
        return;
    }
    if (g_mkdir_with_parents(logger_folder, 0755) != 0) {
        g_printerr("cannot create logger folder %s\n", logger_folder);
        if (logger_purpose != 0) {
            exit(1);
        }
        return;
    }
    logger_format_timestamp(timestamp, sizeof timestamp);
    file = fopen(logger_log_path, "a");
    if (file == NULL) {
        g_printerr("cannot open log file %s\n", logger_log_path);
        if (logger_purpose != 0) {
            exit(1);
        }
        return;
    }
    if (part[0] != '\0') {
        fprintf(file, "[%s] pid:%ld [%s] @%s :] %s\n",
                timestamp, (long)getpid(), logger_level_name(level), part, text);
    } else {
        fprintf(file, "[%s] pid:%ld [%s] :] %s\n",
                timestamp, (long)getpid(), logger_level_name(level), text);
    }
    fclose(file);
}

void Logger::logger_log(LoggerOutputLevel level, const char *component, const char *format, va_list ap)
{
    const char *part = component ? component : "";

    if (level == LOGGER_LEVEL_DEBUG && !logger_get_debug()) {
        return;
    }

    va_list ap_copy;
    va_copy(ap_copy, ap);

    gchar *message = g_strdup_vprintf(format, ap_copy);

    logger_note_dated_path();
    logger_append_log_file(level, part, message);

    if (logger_json_mode) {
        gchar *escaped = logger_json_escape(message ? message : "");
        g_print("{\"timestamp\":%ld,\"level\":\"%s\",\"message\":\"%s\"}\n",
                (long)time(NULL),
                logger_level_name(level),
                escaped);
        g_free(escaped);
    } else if (!logger_get_quiet() &&
               (logger_stderr_level(level) || logger_verbose || debug_env_enabled)) {
        logger_emit_chronicle(level, part, message);
    }

    g_free(message);
    va_end(ap_copy);
}

void Logger::logger_critical(const char *component, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    logger_log(LOGGER_LEVEL_CRITICAL, component, format, args);
    va_end(args);
}
```

`test` and `show` require a stamp. A present stamp that is not eight digits is a different failure. `list` and `show` exit before the init log lines, so they do not append.

```cpp
        if ((logger_purpose == 1 || logger_purpose == 3) && logger_datestamp_arg == NULL) {
            g_printerr("test purpose command requires --log-datestamp YYYYMMDD\n");
            exit(1);
        }
    if (logger_datestamp_arg != NULL && !logger_datestamp_ok(logger_datestamp_arg)) {
        g_printerr("--log-datestamp requires YYYYMMDD\n");
        exit(1);
    }

    if (logger_purpose == 2) {
        logger_list_log_files();
        exit(0);
    }
    if (logger_purpose == 3) {
        logger_show_log_file();
        exit(0);
    }
```

`list` keeps regular `*.log` names. A missing directory returns without creating it. `show` prints the bytes it read and does not add a newline.

```cpp
static gint logger_filename_cmp(gconstpointer left, gconstpointer right)
{
    return strcmp((const char *)left, (const char *)right);
}

void Logger::logger_list_log_files(void)
{
    GDir *dir;
    const char *name;
    GSList *names = NULL;
    GSList *item;
    GError *error = NULL;

    if (logger_folder[0] == '\0') {
        g_printerr("log directory unresolved\n");
        exit(1);
    }
    dir = g_dir_open(logger_folder, 0, &error);
    if (dir == NULL) {
        if (error != NULL && g_error_matches(error, G_FILE_ERROR, G_FILE_ERROR_NOENT)) {
            g_error_free(error);
            return;
        }
        g_printerr("cannot read log directory %s\n", logger_folder);
        if (error != NULL) {
            g_error_free(error);
        }
        exit(1);
    }
    while ((name = g_dir_read_name(dir)) != NULL) {
        char path[4096];

        if (!g_str_has_suffix(name, ".log")) {
            continue;
        }
        if (g_snprintf(path, sizeof path, "%s/%s", logger_folder, name) >= (gint)sizeof path) {
            g_dir_close(dir);
            g_slist_free_full(names, g_free);
            g_printerr("log file path is too long\n");
            exit(1);
        }
        if (!g_file_test(path, G_FILE_TEST_IS_REGULAR)) {
            continue;
        }
        names = g_slist_insert_sorted(names, g_strdup(name), logger_filename_cmp);
    }
    g_dir_close(dir);
    for (item = names; item != NULL; item = item->next) {
        g_print("%s\n", (char *)item->data);
    }
    g_slist_free_full(names, g_free);
}

void Logger::logger_show_log_file(void)
{
    gchar *text = NULL;
    gsize length = 0;
    GError *error = NULL;

    if (logger_log_path[0] == '\0') {
        g_printerr("log file unresolved\n");
        exit(1);
    }
    if (!g_file_get_contents(logger_log_path, &text, &length, &error)) {
        g_printerr("cannot read log file %s\n", logger_log_path);
        if (error != NULL) {
            g_error_free(error);
        }
        exit(1);
    }
    if (length > 0) {
        g_print("%s", text);
    }
    g_free(text);
}
```

## 3. Protection rule

Future editors must not add a second logging API, change the §2.1 line, drop `CRITICAL` from the enumeration, hard-wire `@main` inside the writer, replace §2.0 with `Logger(int argc, char **argv)`, add `logger_set_debug`, put the date in another shape, write the file outside the log directory, or create a temporary file to hold the log.

## 4. Related artifacts

| Artifact | Role |
|----------|------|
| `docs/requirements/index.md` | Registry |
| `docs/requirements/requirement-cpp-output.md` | Streams and quiet for this line |
| `docs/requirements/requirement-cpp-json.md` | The object that replaces this line |
| `docs/requirements/requirement-cpp-logger-folder.md` | Project-log-path is the log directory: `/var/cpp-logger/log` for root or sudo, and `$HOME/.app/cpp-logger/log` for every other user. The test stand-in is that shape under the directory the test command names. The cache path is a different directory. |
| `docs/requirements/requirement-cpp-log-retention.md` | How long a dated day stays plain, compressed, and then gone |
| `docs/requirements/requirement-cpp-logger-portable.md` | The same notebook with a later project's name |
| `docs/requirements/requirement-cpp-tests.md` | The single binary that reads this file |
| `docs/requirements/requirement-cpp-debug-mode.md` | When `logger_debug` is enabled |
| `src/main/cpp/logger.cpp` | `logger_log` and `logger_emit_chronicle` |
| `src/main/cpp/logger.h` | `logger_*` and `LOGGER_LEVEL_*` |

## 5. Status history

| Date | Status | Notes |
|------|--------|-------|
| 2026-11-11 | Active | §2.0 is the ChronicleLogger constructor order. §2.1 is the six-level `log_message` line, with an enumeration for the level and an optional component. The 1.1.1 source still uses `Logger(int argc, char **argv)`, omits `CRITICAL`, and always writes `@main`. |
| 2026-11-11 | Active | Release 1.1.2 constructs `Logger` with the five arguments, includes `LOGGER_LEVEL_CRITICAL`, and writes `@component` only when the caller passes one. |
| 2026-11-11 | Active | Release 1.1.3 hides every human console line while quiet, accepts debug values `show`, `true`, and `1`, and returns the log directory from `logger_persistence_storage`. |

**Last updated**: 2026-11-11
**Owner**: Logger
