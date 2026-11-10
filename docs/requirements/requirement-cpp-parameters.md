**file**: docs/requirements/requirement-cpp-parameters.md
**Status**: Active
**Updated**: 2026-11-10
**Philosophy**: CIAO v2.10.2 / CIAO-Lite (Caution • Intentional • Anti-fragile • Over-engineered / Over-protect)

## 1. Purpose

This file owns every command-line flag and environment variable the binary accepts. `logger_print_help()` must list each of them.

### 1.1 Human-facing

**In one sentence:** You pass flags or environment variables, and `help` prints the same set. `about` prints which program this is, what this computer is, and where it is running from.

| Box | Meaning | Example |
|-----|---------|---------|
| You / this login | The person who starts the program | You pass `--quiet` |
| The other role | `logger_parse_invocation`, then the `Logger` constructor | The parse reads argv and the environment first |
| Not this file | What a log line looks like | `requirement-cpp-logger.md` |

| Includes | Excludes |
|----------|----------|
| The flag table, the environment table, and priority | Console formatting |

| Surface | What you open | What for |
|---------|---------------|----------|
| The terminal | argv and environment | These switches |
| `help` | Usage text | The same list |

| You do… | What it means | What you type |
|---------|---------------|---------------|
| Read the list | Help shows every row below | `cpp-logger help` |
| Read the identity | About prints the three live facts, then stops | `cpp-logger about` |

## 2. Core rules

On a normal run the effective user selects cpp-base-path. `test`, `list`, and `show` do not read `PREFIX`, `HOME`, or the effective user id.

`logger_print_help()` lists every row in both tables, and it lists the two environments. `help` as an argument prints that text and exits 0.

| Parameter | Meaning |
|-----------|---------|
| `help` | Print help and stop. `--json` or `JSON=1` prints one object and no usage text. It does not append a log line |
| `about` | Print the page in `requirement-cpp-about.md` and stop. `--json` or `JSON=1` prints one object and no page. It does not append a log line |
| `test` | cpp-test-purpose-command. Requires `--prefix` or `--home`, and `--log-datestamp`. Writes one `test log` line and stops |
| `list` | cpp-test-purpose-command. Requires `--prefix` or `--home`. Prints regular `*.log` names in that log directory, one per line, sorted. `--json` or `JSON=1` prints one object. Does not append a line |
| `show` | cpp-test-purpose-command. Requires `--prefix` or `--home`, and `--log-datestamp`. Prints that dated file. `--json` or `JSON=1` prints one object. A missing file exits 1 |
| `--verbose` | Display stdout log lines. It works only when the run is not quiet and not JSON (`requirement-cpp-output.md`) |
| `--quiet` | Hide every human console line, including error, critical, and fatal. The dated file still receives the line. `--verbose` and debug do not override this (`requirement-cpp-output.md`) |
| `--json` | JSON mode (`requirement-cpp-json.md`). Every verb prints JSON. `--verbose` does not add a human line |
| `--debug` | cpp-test-purpose-switch, and the normal debug flag. Debug mode shows stdout log lines when the run is not quiet and not JSON (`requirement-cpp-output.md`) |
| `--prefix <dir>` | cpp-test-purpose-switch for root or sudo. cpp-test-log-path is `<dir>/var/cpp-logger/log` |
| `--home <dir>` | cpp-test-purpose-switch for every other user. cpp-test-log-path is `<dir>/.app/cpp-logger/log` |
| `--log-datestamp <YYYYMMDD>` | cpp-test-purpose-switch. Eight digits. The file is `cpp-logger-<YYYYMMDD>.log` (`requirement-cpp-logger.md`) |

| Variable | Meaning |
|----------|---------|
| `DEBUG` | `show`, `true`, or `1`, compared after ASCII lowercasing, enables `logger_debug`. An empty `DEBUG` falls through to `debug` with the same three values. `DEBUG=1` is the documented value. `DEBUG=yes` does not enable debug. Same predicate as `--debug` |
| `QUIET=1` | Same as `--quiet` |
| `JSON=1` | Same as `--json` |
| `HOME` | On a normal run by any user other than root or sudo, the base path is `$HOME/.app/cpp-logger`. `test`, `list`, and `show` ignore it. The binary does not read `PREFIX` for a log directory |

There is no `--shutdown`, no `--bookmark`, no `--config`, and no `SHUTDOWN` or `BOOKMARK` variable.

One invocation accepts one of `test`, `list`, or `show`. A second one exits 1 with `one test purpose command`.

`main` calls `logger_parse_invocation`, then constructs `Logger`. `JSON=1` is read before the scan. `help` and `about` run after the scan, so `--json` after the verb is the same mode as `--json` before it. The C name `logger_output_init` forwards to the constructed object and does not parse again.

### Sample code (this project)

From `src/main/cpp/logger.cpp`.

```cpp
void Logger::logger_set_purpose(int purpose)
{
    if (logger_purpose != 0) {
        g_printerr("one test purpose command\n");
        exit(1);
    }
    logger_purpose = purpose;
}
```

```cpp
        if (strcmp(argv[i], "--quiet") == 0) {
            logger_set_quiet(true);
        } else if (strcmp(argv[i], "--json") == 0) {
            logger_json_mode = true;
        } else if (strcmp(argv[i], "--debug") == 0) {
            debug_env_enabled = true;
        } else if (strcmp(argv[i], "--verbose") == 0) {
            logger_verbose = 1;
        } else if (strcmp(argv[i], "--prefix") == 0) {
            logger_prefix_arg = logger_take_value(argc, argv, &i, "--prefix");
        } else if (strcmp(argv[i], "--home") == 0) {
            logger_home_arg = logger_take_value(argc, argv, &i, "--home");
        } else if (strcmp(argv[i], "--log-datestamp") == 0) {
            logger_datestamp_arg = logger_take_value(argc, argv, &i, "--log-datestamp");
        } else if (strcmp(argv[i], "test") == 0) {
            logger_set_purpose(1);
        } else if (strcmp(argv[i], "list") == 0) {
            logger_set_purpose(2);
        } else if (strcmp(argv[i], "show") == 0) {
            logger_set_purpose(3);
        } else if (strcmp(argv[i], "help") == 0) {
            logger_print_help();
            exit(0);
        }
```

## 3. Protection rule

Future editors must not add a flag in `logger.cpp` without adding the same flag to this table and to `logger_print_help()` in the same change. Quiet after startup is `logger_set_quiet`, owned by `requirement-cpp-output.md`. JSON mode is owned by `requirement-cpp-json.md`. Debug after startup is `logger_get_debug`, owned by `requirement-cpp-debug-mode.md`. cpp-base-path and cpp-log-path are owned by `requirement-cpp-logger-folder.md`. The dated file is owned by `requirement-cpp-logger.md`. The test command check is owned by `requirement-cpp-tests.md`.

## 4. Related artifacts

| Artifact | Role |
|----------|------|
| `docs/requirements/index.md` | Registry |
| `docs/requirements/requirement-cpp-logger.md` | The human line and the dated file `--log-datestamp` names |
| `docs/requirements/requirement-cpp-output.md` | Default hidden stdout, `--verbose`, and quiet |
| `docs/requirements/requirement-cpp-json.md` | JSON mode implies quiet |
| `docs/requirements/requirement-cpp-debug-mode.md` | What `DEBUG` and `--debug` turn on |
| `docs/requirements/requirement-cpp-logger-folder.md` | cpp-base-path, cpp-log-path, and the test stand-ins |
| cpp-test-purpose-command | The verbs `test`, `list`, and `show` |
| cpp-test-purpose-switch | `--prefix`, `--home`, `--debug`, `--log-datestamp` |
| `docs/requirements/requirement-cpp-tests.md` | The `test` verb |
| `src/main/cpp/logger.cpp` | Parser and help text |
| `docs/requirements/requirement-cpp-about.md` | The about page |

**Last updated**: 2026-11-10
**Owner**: Parameters
