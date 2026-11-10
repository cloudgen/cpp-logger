# C++ coding style

**file**: docs/requirements/requirement-cpp-coding-style.md
**Status**: Active
**Updated**: 2026-11-10

## 1. Purpose

This file is where portable C++ lessons are specialized for this product. Without this requirement, those lessons arrive raw. Class law points here and does not copy these rules.

The ship unit does not link GTK or WebKit. Callback rules for `g_signal_connect` are not product law.

The tree comes from cpp-template and cpp-gnome-template. The primary language is C++.

### 1.1 Human-facing

**In one sentence:** You change C++ in this repository by following the file, linkage, naming, and logging rules in this file.

| Box | Meaning | Example |
|-----|---------|---------|
| You / this login | The person editing source | You add a function in a `.cpp` file |
| The other role | The compiler and linker | They reject a `static` function that another file calls |
| Not this file | Which flags exist | `requirement-cpp-parameters.md` |

| Includes | Excludes |
|----------|----------|
| `extern "C"`, comment shape, the present `logger_*` names, kebab-case and camel-case, quiet and debug accessors | Which files belong in `src/main/cpp` (`requirement-cpp-folder.md`). The class map (`requirement-cpp-oop.md`). Gradle banners (`requirement-cpp-gradle-coding-style.md`). Ant banners (`requirement-cpp-ant-coding-style.md`). Meson banners (`requirement-cpp-meson-coding-style.md`) |

| Surface | What you open | What for |
|---------|---------------|----------|
| `src/main/cpp/*.cpp` | Product source | The rules below |
| `src/main/cpp/logger.h` | Logging API | The only console output |
| kebab-case | Hyphen spelling | Path tokens |
| camel-case | Capital-letter spelling | C++ names |

| You do… | What it means | What you type |
|---------|---------------|---------------|
| Share a logger function across files | Leave it non-static and declare it inside `extern "C"` in `logger.h` | `logger_set_quiet` |
| Name a path token | Use kebab-case | `log-dir` |
| Name a C++ type | Use upper camel-case | `LoggerOutputLevel` |

## 2. Core rules

This product follows **CIAO-Lite**:
- **C** - Caution
- **I** - Intentional
- **A** - Anti-fragile
- **O** - Over-protect

**Priority**: Safety, clarity of intention, and long-term maintainability always have higher priority than simplicity or minimalism.

> **NO hard-coded URLs, paths, project names, filenames, or service-specific strings are allowed anywhere in the source code.**
> All such values **MUST** come from `project.h` (Single Source of Truth) or from a requirement that names the value.
> Violating this rule breaks forking and CIAO-Lite.

## File types

Membership of `src/main/cpp` is owned by `requirement-cpp-folder.md`. That requirement names the directory, the four ship-unit files, and the rule that a translation unit is `.cpp` unless it names a `.c` file. This file does not keep a second copy of that list.

## Naming

Glossary words, not restated here as a second definition:

| Spelling | Term | Use it for |
|----------|------|------------|
| `log-dir` | [kebab-case](../terminologies/kebab-case.md) | A multi-word path segment, log-file stem, or product token the program creates |
| `logDir` | [camel-case](../terminologies/camel-case.md), lower form | A C++ method or a C++ local |
| `LogDir` | [camel-case](../terminologies/camel-case.md), upper form | A C++ type, class, struct, or enum |
| `log_dir` | snake_case (not those two terms) | An `extern "C"` function, and a function in a `.c` file that a requirement names |

Rules:

1. A C++ type uses upper camel-case. A C++ method or local uses lower camel-case.
2. An `extern "C"` function stays snake_case so the C linker name matches. The present logger surface is `logger_*`. That surface is not the allowed end state. The class map is `requirement-cpp-oop.md`. A new logging behavior is a method on `Logger` before it is a new `logger_*` function.
3. A multi-word path segment uses kebab-case. Do not put a camel-case token or a snake_case token in that segment. The directories themselves are cpp-base-path and cpp-log-path, owned by `requirement-cpp-logger-folder.md`. This file does not choose them.
4. Do not put a hyphen in a C++ identifier. Do not put a capital letter in a kebab-case token.
5. Do not introduce `ciao_` or `CIAO_` names.

## Cross-file function visibility

- A `static` function can be called only from the same source file.
- A function declared in a header and called from another source file is not `static`.
- A file-local helper is `static`.

There is no GTK signal callback in this product. Do not add `g_signal_connect` rules back into this file.

## C++ linkage (`extern "C"`)

The logger header is included from C++. It declares `class Logger`. It keeps C linkage for the `logger_*` names, and those names forward to the object `main` constructed. `logger_log` takes `va_list` by value. The body in `src/main/cpp/logger.cpp` uses `va_copy` before `g_strdup_vprintf`. That body is the sample in `requirement-cpp-logger.md`.

Sample code (this project), from `src/main/cpp/logger.h`:

```cpp
#ifndef LOGGER_H
#define LOGGER_H

#include <stdbool.h>
#include <stdarg.h>
#include <glib.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    LOGGER_LEVEL_INFO,
    LOGGER_LEVEL_WARNING,
    LOGGER_LEVEL_ERROR,
    LOGGER_LEVEL_DEBUG,
    LOGGER_LEVEL_FATAL
} LoggerOutputLevel;

void logger_log(LoggerOutputLevel level, const char *format, va_list ap);
void logger_output_init(int argc, char **argv);
int logger_is_test_verb(void);
void logger_set_quiet(bool quiet);
bool logger_get_quiet(void);
bool logger_get_debug(void);
void logger_info(const char *format, ...);
void logger_warn(const char *format, ...);
void logger_error(const char *format, ...);
void logger_fatal(const char *format, ...);
void logger_debug(const char *format, ...);
void logger_print_help(void);

#ifdef __cplusplus
}
#endif

#endif
```

A C++ type that the logger header names, including `LoggerOutputLevel`, may sit inside that guard. A C++ type that no C caller names does not need the guard.

## Comment style

Gradle task banners are `requirement-cpp-gradle-coding-style.md`. Ant target banners are `requirement-cpp-ant-coding-style.md`. Meson banners are `requirement-cpp-meson-coding-style.md`. This section is the C++ block.

- Every function must have:
  - Purpose
  - Parameters
  - Dependencies
  - Why This Design
  - CIAO Principles Applied
  - Protection Rule (if applicable)
- Use clear section dividers: `──────────────────────────────────────────────`

## Output and logging

- Product code calls `logger_info()`, `logger_warn()`, `logger_error()`, `logger_fatal()`, and `logger_debug()` only for lines.
- Quiet mode is `logger_set_quiet(bool)` and `logger_get_quiet()`. Debug is `logger_get_debug()`. There is no `logger_set_debug`.
- `printf`, `fprintf`, `g_print`, and `perror` are allowed inside `logger.cpp` only.
- The human line is `requirement-cpp-logger.md`. Quiet is `requirement-cpp-output.md`. JSON mode is `requirement-cpp-json.md`. `DEBUG` is `requirement-cpp-debug-mode.md`. Flag names are `requirement-cpp-parameters.md`.

## Configuration

- **NO hardcoded URLs, paths, project names, filenames, or behaviors in source code.**
- Identity values come from `project.h`.

## File organization

- Which files sit in `src/main/cpp` is `requirement-cpp-folder.md`.
- A header and its translation unit stay paired.
- `project.h` is the single source of truth for basic metadata only.

---

**Last Updated**: 2026-11-10
**Owner**: CIAO-Lite Style Guide
