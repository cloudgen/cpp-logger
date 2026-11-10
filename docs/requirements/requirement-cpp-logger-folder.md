**file**: docs/requirements/requirement-cpp-logger-folder.md
**Status**: Active
**Updated**: 2026-10-04
**Philosophy**: CIAO v2.10.2 / CIAO-Lite (Caution • Intentional • Anti-fragile • Over-engineered / Over-protect)

## 1. Purpose

This file owns the application directory and the log directory for this C++ project. The dated file is `requirement-cpp-logger.md`. Install bins are `requirement-cpp-global-installation.md` and `requirement-cpp-local-installation.md`.

Project-base-path is the application directory for the current environment. Root or sudo uses `/var/cpp-logger`. Every other user uses `$HOME/.app/cpp-logger`. The log directory is not this path. The cache directory is not this path. The install bin is not this path.

Project-log-path is the log directory. It is the project-base-path plus `/log`. Root or sudo uses `/var/cpp-logger/log`. Every other user uses `$HOME/.app/cpp-logger/log`. The dated file sits inside this directory.

The project's cache path is a different directory. A normal user uses `$HOME/.cache/cache-cpp-logger/` or `/dev/shm/cache/cache-cpp-logger`. Root or sudo uses `/var/cache/cpp-logger/`. This file does not create that cache directory.

The test command uses a stand-in. `--prefix <dir>` names `<dir>/var/cpp-logger` and `<dir>/var/cpp-logger/log`. `--home <dir>` names `<dir>/.app/cpp-logger` and `<dir>/.app/cpp-logger/log`.

### 1.1 Human-facing

**In one sentence:** Root or sudo logs under `/var/cpp-logger/log`, and every other user logs under `$HOME/.app/cpp-logger/log`.

| Box | Meaning | Example |
|-----|---------|---------|
| You / this login | The person whose environment selects the directory | A normal user, or root or sudo |
| The other role | `logger.cpp` | It resolves one log directory and `--debug` prints it |
| Not this file | The dated file name and the ship-unit source directory | `requirement-cpp-logger.md` and `requirement-cpp-folder.md` |

| Includes | Excludes |
|----------|----------|
| The two base paths and the two log directories | `$PREFIX/bin` and `$HOME/.local/bin` |

| Surface | What you open | What for |
|---------|---------------|----------|
| cpp-log-path | The log directory | Find the dated file |
| `--debug` | Two debug lines | Read `logger folder` and `log file` |

For a normal run, `{{app-name}}` is `cpp-logger`. A later project uses its own name in the same places. That rule is `requirement-cpp-logger-portable.md`.

| Environment | cpp-base-path | cpp-log-path |
|-------------|---------------|--------------|
| Root or sudo | `/var/cpp-logger` | `/var/cpp-logger/log` |
| Other | `$HOME/.app/cpp-logger` | `$HOME/.app/cpp-logger/log` |

For the test command, the literal directory `/tmp` stands in:

| Switch | cpp-test-base-path | cpp-test-log-path |
|--------|--------------------|-------------------|
| `--prefix /tmp` | `/tmp/var/cpp-logger` | `/tmp/var/cpp-logger/log` |
| `--home /tmp` | `/tmp/.app/cpp-logger` | `/tmp/.app/cpp-logger/log` |

## 2. Core rules

1. There are two environments. Root or sudo is a process whose effective user id is 0, including a process started with sudo. Every other effective user is the other environment.
2. cpp-base-path for root or sudo is `/var/cpp-logger`. cpp-log-path is `/var/cpp-logger/log`. cpp-base-path for every other user is `$HOME/.app/cpp-logger`. cpp-log-path is `$HOME/.app/cpp-logger/log`.
3. A normal run uses those two paths. It does not read `--prefix`, `--home`, or `PREFIX` to choose a directory. When the other user's `HOME` is empty, the directory stays unresolved. The program does not call `getpwuid` and does not invent a path.
4. The test command uses cpp-test-base-path and cpp-test-log-path. `--prefix <dir>` names `<dir>/var/cpp-logger` and `<dir>/var/cpp-logger/log`. `--home <dir>` names `<dir>/.app/cpp-logger` and `<dir>/.app/cpp-logger/log`. `--prefix /tmp` therefore names `/tmp/var/cpp-logger/log`. `--home /tmp` names `/tmp/.app/cpp-logger/log`. Those two directories are different. `--prefix` wins when both switches are present. The test command does not read `PREFIX`, `HOME`, or the effective user id.
5. `--debug`, or a non-empty `DEBUG`, prints `logger folder <path>` for the log directory and `log file <path>` for the dated file. When the directory is unresolved the line is `logger folder unresolved`. Quiet still hides a human debug line.
6. `--debug` does not create the directory. The directory is created only when a log line is appended, which is `requirement-cpp-logger.md`. The program does not use `mkstemp`.

### Sample code (this project)

From `src/main/cpp/logger.cpp`. `PROJECT_NAME` is the macro in `project.h`. A test-purpose command uses the directory switch. A normal run uses the effective user id and, for every user other than root or sudo, `HOME`. The missing-switch check and the path choice are separate blocks in the `Logger` constructor.

```cpp
void Logger::logger_join_base(char *out, size_t out_size, const char *head, const char *tail)
{
    gchar *trimmed;
    gsize length;
    gint wrote;

    if (head == NULL || head[0] == '\0' || strchr(head, '\n') != NULL || strchr(head, '\r') != NULL) {
        g_printerr("logger folder directory is empty or not one line\n");
        exit(1);
    }
    trimmed = g_strdup(head);
    length = strlen(trimmed);
    while (length > 1 && trimmed[length - 1] == '/') {
        trimmed[--length] = '\0';
    }
    if (strcmp(trimmed, "/") == 0) {
        wrote = g_snprintf(out, out_size, "/%s", tail);
    } else {
        wrote = g_snprintf(out, out_size, "%s/%s", trimmed, tail);
    }
    g_free(trimmed);
    if (wrote < 0 || (size_t)wrote >= out_size) {
        g_printerr("logger folder path is too long\n");
        exit(1);
    }
}
```

```cpp
        if (logger_prefix_arg == NULL && logger_home_arg == NULL) {
            g_printerr("test purpose command requires --prefix or --home\n");
            exit(1);
        }
        if (logger_purpose != 0) {
            const char *dir = logger_prefix_arg != NULL ? logger_prefix_arg : logger_home_arg;
            if (g_snprintf(tail, sizeof tail, "%s/%s",
                           logger_prefix_arg != NULL ? "var" : ".app",
                           PROJECT_NAME) >= (gint)sizeof tail) {
                g_printerr("logger folder path is too long\n");
                exit(1);
            }
            logger_join_base(base, sizeof base, dir, tail);
            logger_apply_base(base, stamp);
        } else if (geteuid() == 0) {
            if (g_snprintf(base, sizeof base, "/var/%s", PROJECT_NAME) >= (gint)sizeof base) {
                g_printerr("logger folder path is too long\n");
                exit(1);
            }
            logger_apply_base(base, stamp);
        } else {
            const char *home = getenv("HOME");
            if (home != NULL && home[0] != '\0') {
                if (g_snprintf(tail, sizeof tail, ".app/%s", PROJECT_NAME) >= (gint)sizeof tail) {
                    g_printerr("logger folder path is too long\n");
                    exit(1);
                }
                logger_join_base(base, sizeof base, home, tail);
                logger_apply_base(base, stamp);
            }
        }
```

## 3. Protection rule

Future editors must not invent a third environment, point this directory at an install bin, point this directory at the cache path, fall back to another home when `HOME` is empty, or treat `src/main/cpp` as the log directory.

## 4. Related artifacts

| Artifact | Role |
|----------|------|
| `docs/requirements/index.md` | Registry |
| Project-base-path | The application directory. Root or sudo uses `/var/cpp-logger`. Every other user uses `$HOME/.app/cpp-logger`. |
| Project-log-path | That directory plus `/log`: `/var/cpp-logger/log` or `$HOME/.app/cpp-logger/log`. |
| Cache path | A different directory. A normal user uses `$HOME/.cache/cache-cpp-logger/` or `/dev/shm/cache/cache-cpp-logger`. Root or sudo uses `/var/cache/cpp-logger/`. |
| Test stand-in | `--prefix <dir>` names `<dir>/var/cpp-logger` and `<dir>/var/cpp-logger/log`. `--home <dir>` names `<dir>/.app/cpp-logger` and `<dir>/.app/cpp-logger/log`. |
| `docs/requirements/requirement-cpp-logger.md` | The file inside the log directory |
| `docs/requirements/requirement-cpp-folder.md` | Ship-unit sources, a different directory |
| `docs/requirements/requirement-cpp-tests.md` | One test binary and the test command |
| `src/main/cpp/logger.cpp` | Resolver |

**Last updated**: 2026-10-04
**Owner**: Logger folder
