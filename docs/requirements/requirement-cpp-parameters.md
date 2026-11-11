**file**: docs/requirements/requirement-cpp-parameters.md
**Status**: Active
**Updated**: 2026-11-11
**Philosophy**: CIAO v2.10.2 / CIAO-Lite (Caution • Intentional • Anti-fragile • Over-engineered / Over-protect)

## 1. Purpose

This file owns every command-line flag and environment variable the binary accepts. `Help::printPage()` lists each of them. `Logger::logger_print_help()` forwards to that page.

### 1.1 Human-facing

**In one sentence:** You pass flags or environment variables, and `help` prints the same set. `about` prints which program this is, what this computer is, and where it is running from.

| Box | Meaning | Example |
|-----|---------|---------|
| You / this login | The person who starts the program | You pass `--quiet` |
| The other role | `About::parse`, then `About` routes one verb | The parse stores argv and the environment on `About` |
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

`Help::printPage()` lists every row in both tables, and it lists the two environments. `help` as an argument prints that text and exits 0. The page reads the program name from `About`.

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

`About::parse` reads argv and the environment and stores them on `About`. `JSON=1` is read before the scan. The scan resets the flags and reads the environment again, so a value set for this process is still seen. `help` and `about` are stored during the scan and run after it, so `--json` after the verb is the same mode as `--json` before it. The scan does not exit 0 for `help` or `about`. `About::run` prints the page and returns 0. The C name `logger_output_init` forwards to the constructed logger and does not parse again.

### Sample code (this project)

From `src/main/cpp/about.cpp`. The scan stores the page. It does not print help inside the scan.

```cpp
void About::setPurpose(int next)
{
    if (purpose != 0 || page != 0) {
        die("one test purpose command");
    }
    purpose = next;
}
```

```cpp
        if (strcmp(argv[i], "--quiet") == 0) {
            sawQuiet = 1;
            quietMode = true;
        } else if (strcmp(argv[i], "--json") == 0) {
            sawJson = 1;
            jsonMode = true;
        } else if (strcmp(argv[i], "--debug") == 0) {
            sawDebug = 1;
            debugEnabled = true;
        } else if (strcmp(argv[i], "--verbose") == 0) {
            sawVerbose = 1;
            verbose = 1;
        } else if (strcmp(argv[i], "--prefix") == 0) {
            prefix = takeValue(argc, argv, &i, "--prefix");
        } else if (strcmp(argv[i], "--home") == 0) {
            home = takeValue(argc, argv, &i, "--home");
        } else if (strcmp(argv[i], "--log-datestamp") == 0) {
            datestamp = takeValue(argc, argv, &i, "--log-datestamp");
        } else if (strcmp(argv[i], "test") == 0) {
            setPurpose(1);
        } else if (strcmp(argv[i], "list") == 0) {
            setPurpose(2);
        } else if (strcmp(argv[i], "show") == 0) {
            setPurpose(3);
        } else if (strcmp(argv[i], "help") == 0) {
            setPage(1);
        } else if (strcmp(argv[i], "about") == 0) {
            setPage(2);
        }
```

## 3. Protection rule

Future editors must not add a flag in `About::parse` without adding the same flag to this table and to `Help::printPage()` in the same change. Quiet after startup is `logger_set_quiet`, owned by `requirement-cpp-output.md`. JSON mode is owned by `requirement-cpp-json.md`. Debug after startup is `logger_get_debug`, owned by `requirement-cpp-debug-mode.md`. cpp-base-path and cpp-log-path are owned by `requirement-cpp-logger-folder.md`. The dated file is owned by `requirement-cpp-logger.md`. The test command check is owned by `requirement-cpp-tests.md`.

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
| `src/main/cpp/about.cpp` | `About::parse` stores the flags and the command |
| `src/main/cpp/help.cpp` | The usage page |
| `docs/requirements/requirement-cpp-about.md` | The about page |

**Last updated**: 2026-11-11
**Owner**: Parameters
