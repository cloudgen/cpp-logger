**file**: docs/requirements/requirement-cpp-output.md
**Status**: Active
**Updated**: 2026-11-11
**Philosophy**: CIAO v2.10.2 / CIAO-Lite (Caution • Intentional • Anti-fragile • Over-engineered / Over-protect)

## 1. Purpose

This file owns the console channel. Every product message goes through the logger. This file owns quiet, `--verbose`, and which stream a human line uses. The line and the dated file are `requirement-cpp-logger.md`. The JSON object is `requirement-cpp-json.md`.

### 1.1 Human-facing

**In one sentence:** Stdout log lines stay hidden until you pass `--verbose` or turn debug mode on, and neither one shows them while the run is quiet or JSON.

| Box | Meaning | Example |
|-----|---------|---------|
| You / this login | The person watching the terminal | You pass `--verbose` |
| The other role | `logger.cpp` | It prints with `g_print` or `g_printerr`, or it suppresses the line |
| Not this file | The human line shape and the JSON object | `requirement-cpp-logger.md` and `requirement-cpp-json.md` |

| Includes | Excludes |
|----------|----------|
| One output path, the human-line streams, quiet, and `--verbose` | The flag spelling list (`requirement-cpp-parameters.md`) and the JSON object |

| Surface | What you open | What for |
|---------|---------------|----------|
| The terminal | stdout and stderr | Human lines |
| `src/main/cpp/logger.cpp` | Implementation | `logger_set_quiet` |

| You do… | What it means | What you type |
|---------|---------------|---------------|
| Leave the default | Stdout log lines stay hidden. The error, critical, and fatal lines on stderr stay | `cpp-logger` |
| Show stdout log lines | Info, warning, and debug human lines appear. Quiet and JSON still hide them | `--verbose` or `--debug` |
| Hide them again | `--verbose` does not bring stdout lines back | `--verbose --quiet` |

## 2. Core rules

- Product code calls `logger_*` only. `printf`, `fprintf`, `g_print`, and `perror` are allowed inside `logger.cpp` only.
- A human line uses the streams below. `logger_emit_chronicle` is that writer.

| Level | Stream |
|-------|--------|
| `INFO`, `WARNING`, `DEBUG` | stdout (`g_print`) |
| `ERROR`, `CRITICAL`, `FATAL` | stderr (`g_printerr`) |

- `logger_set_quiet(bool)` stores quiet mode. `logger_get_quiet()` returns that flag. The flags are not exported from `logger.h`. There is no `logger_set_verbose`. `--verbose` stays inside `logger.cpp`.
- Stdout human lines (`INFO`, `WARNING`, and `DEBUG`) are hidden by default. `--verbose` displays those lines only when the run is not quiet and not JSON. Debug mode (`--debug` or a non-empty `DEBUG`) displays those same lines only when the run is not quiet and not JSON. Quiet keeps them hidden. JSON mode keeps them hidden. Neither switch overrides quiet or JSON.
- The human `ERROR`, `CRITICAL`, and `FATAL` lines use stderr and stay visible without `--verbose`. Quiet does not hide them. `CRITICAL` is `LOGGER_LEVEL_CRITICAL` in `requirement-cpp-logger.md`.
- Hiding stdout does not skip the dated file. The human line is still appended, which is `requirement-cpp-logger.md`. `help` is not a log line and still prints.
- While JSON mode is on, this human line is not what the terminal shows. `requirement-cpp-json.md` owns that mode. JSON mode implies quiet, and a call that would clear quiet does not clear it. `--verbose` does not add the human line beside the JSON object.
- `logger_get_debug()` is declared beside the quiet accessors. When debug is enabled is `requirement-cpp-debug-mode.md`. There is no `logger_set_debug`. A debug event is still a stdout log line. Debug mode shows it, and `--verbose` shows it, while the run is not quiet and not JSON.
- `list` and `show` print file names and file bytes. Those prints are not log lines. Quiet and a missing `--verbose` do not hide them. JSON mode replaces them with the object in `requirement-cpp-json.md`. What they print is `requirement-cpp-logger.md`.

### Sample code (this project)

From `src/main/cpp/logger.cpp`. The 1.1.2 source returns true from `logger_stderr_level` for `LOGGER_LEVEL_ERROR`, `LOGGER_LEVEL_CRITICAL`, and `LOGGER_LEVEL_FATAL`. The full `logger_log` body is the sample in `requirement-cpp-logger.md`. The append runs before this test.

```cpp
void logger_set_quiet(bool quiet)
{
    if (logger_json_mode) {
        logger_quiet_mode = true;
        return;
    }
    logger_quiet_mode = quiet;
}

bool logger_get_quiet(void)
{
    return logger_quiet_mode;
}

int Logger::logger_stderr_level(LoggerOutputLevel level)
{
    return level == LOGGER_LEVEL_ERROR || level == LOGGER_LEVEL_CRITICAL || level == LOGGER_LEVEL_FATAL;
}
```

```cpp
    logger_append_log_file(level, part, message);

    if (logger_json_mode) {
        gchar *escaped = logger_json_escape(message ? message : "");
        g_print("{\"timestamp\":%ld,\"level\":\"%s\",\"message\":\"%s\"}\n",
                (long)time(NULL),
                logger_level_name(level),
                escaped);
        g_free(escaped);
    } else if (logger_stderr_level(level) ||
               (!logger_get_quiet() && (logger_verbose || debug_env_enabled))) {
        logger_emit_chronicle(level, part, message);
    }
```

## 3. Protection rule

Future editors must not export the quiet flag, print a product line from outside `logger.cpp`, let quiet hide a human `ERROR`, `CRITICAL`, or `FATAL` line, or let `--verbose` reveal a stdout log line while quiet or JSON is on.

## 4. Related artifacts

| Artifact | Role |
|----------|------|
| `docs/requirements/index.md` | Registry |
| `docs/requirements/requirement-cpp-logger.md` | The human line |
| `docs/requirements/requirement-cpp-json.md` | JSON mode, which implies quiet |
| `docs/requirements/requirement-cpp-parameters.md` | How `--verbose` and `--quiet` are spelled |
| `src/main/cpp/logger.cpp` | Streams and `logger_set_quiet` |
| `src/main/cpp/logger.h` | `logger_set_quiet` and `logger_get_quiet` |

**Last updated**: 2026-11-11
**Owner**: Output
