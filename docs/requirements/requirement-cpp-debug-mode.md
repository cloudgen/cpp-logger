**file**: docs/requirements/requirement-cpp-debug-mode.md
**Status**: Active
**Updated**: 2026-11-11
**Philosophy**: CIAO v2.10.2 / CIAO-Lite (Caution • Intentional • Anti-fragile • Over-engineered / Over-protect)

## 1. Purpose

This file owns when `logger_debug` prints. The call stays in `requirement-cpp-logger.md`. Quiet stays in `requirement-cpp-output.md`. JSON mode stays in `requirement-cpp-json.md`. The flag stays in `requirement-cpp-parameters.md`.

### 1.1 Human-facing

**In one sentence:** You start the program with `--debug`, or with `DEBUG` or `debug` set to `show`, `true`, or `1`, and the terminal shows the log lines, including each found switch and the start and end of component `main`.

| Box | Meaning | Example |
|-----|---------|---------|
| You / this login | The person inspecting startup | You launch with debug on |
| The other role | `logger.cpp` | It reads `DEBUG`, then `debug`, once during init |
| Not this file | How quiet and JSON hide lines | `requirement-cpp-output.md` and `requirement-cpp-json.md` |

| Includes | Excludes |
|----------|----------|
| The predicate that enables `logger_debug` | A second, separate debug predicate |

| Surface | What you open | What for |
|---------|---------------|----------|
| The environment | `DEBUG` or `debug` | Turn debug lines on with `show`, `true`, or `1` |
| The terminal | Debug lines | Read them |

## 2. Core rules

1. `--debug` enables `logger_debug`. The same predicate is true when `DEBUG` is `show`, `true`, or `1`, compared after ASCII lowercasing. An empty `DEBUG` falls through to the variable `debug` with the same three values. Any other value, including `yes` or a bare non-empty string, leaves the predicate off. An empty or unset pair, with no `--debug`, leaves it off. `DEBUG=1` is the documented environment value. `logger_parse_invocation` and the `Logger` constructor both read that pair. There is no second predicate.
2. `logger_get_debug()` returns that captured flag. There is no `logger_set_debug`.
3. Quiet mode hides the human debug line even when debug is on. Debug mode shows stdout log lines while the run is not quiet and not JSON. `--verbose` shows them the same way. In JSON mode that same call is the JSON object from `requirement-cpp-json.md` when this predicate is on. `--verbose` and `--debug` do not add the human line in JSON mode.
4. When debug is on, the first debug lines name each switch that was present on the command. A switch that was not passed is not named. The text is `switch --quiet`, `switch --json`, `switch --debug`, `switch --verbose`, `switch --prefix <dir>`, `switch --home <dir>`, or `switch --log-datestamp <YYYYMMDD>`. `DEBUG`, `QUIET`, and `JSON` are environment variables, so they are not switch lines. `help`, `about`, `test`, `list`, and `show` are commands, so they are not switch lines.
5. When debug is on, the component `main` logs `component main start` and later `component main end`. `help` and `about` exit before those lines. `list` and `show` exit before those lines. A normal run and the `test` command emit the pair. The screen follows rule 3. The dated file still receives the line.
6. When debug is on and the log directory is resolved, the program prints `logger folder <path>` for cpp-log-path, or for cpp-test-log-path when the command is `test`, and `log file <path>` for the dated file. Those paths are `requirement-cpp-logger-folder.md` and `requirement-cpp-logger.md`. When the directory is unresolved the line is `logger folder unresolved`. The program does not print userscript fields `enabled`, `state`, `path`, `priority`, or `match`.

### Sample code (this project)

From `src/main/cpp/logger.cpp`. Whether the human debug line is shown is `requirement-cpp-output.md`.

```cpp
bool logger_get_debug(void)
{
    return debug_env_enabled;
}

void logger_debug(const char *format, ...)
{
    if (!logger_get_debug()) return;

    va_list args;
    va_start(args, format);
    logger_log(LOGGER_LEVEL_DEBUG, format, args);
    va_end(args);
}
```

```cpp
    if (level == LOGGER_LEVEL_DEBUG && !logger_get_debug()) {
        return;
    }
```

```cpp
    if (debug_env_enabled) {
        if (logger_folder[0] != '\0') {
            logger_debug("logger folder %s", logger_folder);
            logger_debug("log file %s", logger_log_path);
        } else {
            logger_debug("logger folder unresolved");
        }
    }
```

## 3. Protection rule

Future editors must not add a second debug predicate beside `--debug` and the `show` / `true` / `1` check on `DEBUG` and `debug`, must not treat any other non-empty `DEBUG` as on, and must not add `logger_set_debug`.

## 4. Related artifacts

| Artifact | Role |
|----------|------|
| `docs/requirements/index.md` | Registry |
| `docs/requirements/requirement-cpp-logger.md` | `logger_debug` and the dated file that line names |
| `docs/requirements/requirement-cpp-output.md` | Quiet hides the human debug line |
| `docs/requirements/requirement-cpp-json.md` | Debug in JSON mode is a JSON object |
| `docs/requirements/requirement-cpp-logger-folder.md` | The folder line debug prints |
| `docs/requirements/requirement-cpp-parameters.md` | The `--debug` and `DEBUG` rows |
| `src/main/cpp/logger.cpp` | The predicate and `logger_get_debug` |

**Last updated**: 2026-11-11
**Owner**: Debug mode
