**file**: docs/requirements/requirement-cpp-build-sh-coding-style.md
**Status**: Active
**Updated**: 2026-11-10
**Philosophy**: CIAO v2.10.2 / CIAO-Lite (Caution • Intentional • Anti-fragile • Over-engineered / Over-protect)

## 1. Purpose

This file owns how `build.sh` is written. Without this file, the portable shell lessons arrive raw and agents treat a shell-program style sheet as the law of this build. The interpreter, the shebang, and the verb list stay in `requirement-cpp-build-sh.md`. C++ comment blocks stay in `requirement-cpp-coding-style.md`.

### 1.1 Human-facing

**In one sentence:** You still type `./build.sh build`, and every function inside the script has a `build_` name and a purpose header.

| Box | Meaning | Example |
|-----|---------|---------|
| You / this login | The person who compiles | `./build.sh build` |
| The other role | `build.sh` | Its functions use the names in §2.1 |
| Not this file | The `#!/bin/sh` line, the package list, and the C++ compile flags | `requirement-cpp-build-sh.md` and `requirement-cpp-dependencies.md` |

| Includes | Excludes |
|----------|----------|
| The `build_` prefix, the function header, and the rule that typed verbs stay short | A shell menu, `out_` printers, and a second copy of the compile line |

| Surface | What you open | What for |
|---------|---------------|----------|
| `build.sh` | Functions and the `case` | Prefixes and headers |
| A shell | The verbs you type | `install`, `clean`, and `build` |

| You do… | What it means | What you type |
|---------|---------------|---------------|
| Build | The `case` calls `build_link` | `./build.sh build` |

## 2. Core rules

1. A function in `build.sh` starts with `build_` and continues in snake_case. A typed verb stays the short word in `requirement-cpp-build-sh.md`. The `case` maps that word to the function. The script has no bare function named `install`, `clean`, `build`, `main`, or `help`.
2. The functions are `build_install_deps`, `build_place_binary`, `build_install_local`, `build_install_global`, `build_clean`, and `build_link`. `install` and `deps` call `build_install_deps`. `install-local` calls `build_install_local`. `install-global` calls `build_install_global`. `clean` calls `build_clean`. `build`, `simple`, and an empty argument call `build_link`.
3. Every function has the header in §2.2 directly above it. The labels, in this order, are `GENERAL PURPOSE`, `CIAO PRINCIPLES APPLIED`, and `Last reviewed`. The first comment line names the function and one purpose. The divider is a line of `=`.
4. `build_link` keeps the protection block that forbids rewriting the compile and link lines. The header in §2.2 sits with that block. The `pkg-config` line, the `gcc` line, and the `g++` line stay unquoted where the shell must split the flag list and the file list.
5. Other expansions in `build.sh` are quoted. The script does not use `[[`, `source`, a shell array, process substitution, a here-string, or the `function` keyword. `local` inside a function stays allowed. Those bans are also `requirement-cpp-build-sh.md`.
6. Messages stay `echo` lines that begin with `[INFO]`, `[WARN]`, `[ERROR]`, or `[DEBUG]`. This file does not add `out_info`, `out_warn`, `out_die`, or a JSON printer. The logger ship unit is not this script.
7. `project.sh` keeps `apt_install`, `apt_uninstall`, and `install`. Ant and Gradle emit those names. This file does not rename them.
8. The proof is `tests/test-cpp-builds.sh`, id `TP-CPP-BUILD-SH-STYLE`. Mark that id pass only after the script exits 0.

### 2.1 Function names

| Typed verb | Function |
|------------|----------|
| `install`, `deps` | `build_install_deps` |
| `install-local` | `build_install_local` |
| `install-global` | `build_install_global` |
| `clean` | `build_clean` |
| `build`, `simple`, empty | `build_link` |
| (called by the place verbs) | `build_place_binary` |

### 2.2 Function header

```text
# =============================================================================
# build_name() - One purpose line
# =============================================================================
#
# GENERAL PURPOSE:
# What the function does and why it exists.
#
# CIAO PRINCIPLES APPLIED:
# - Caution: ...
# - Intentional: ...
#
# Last reviewed: YYYY-MM-DD
# =============================================================================
```

### 2.3 Implementation notes (this project)

The six functions in §2.1 are the live set in `build.sh`. `Last reviewed` on those headers is `2026-11-10`. The compile lines inside `build_link` still pass `glib-2.0` to `pkg-config` and link with `-pthread`.

## 3. Design principles

- **Caution**: a new helper without `build_` is a style failure, not a new verb.
- **Intentional**: the header says why the function exists, so a later edit does not treat the shell lessons as unstated.
- **Anti-fragile**: the typed verbs stay stable when a function is renamed behind them.
- **Over-protect**: the compile protection block stays on `build_link`.

## 4. Protection rule

Future editors must not drop the `build_` prefix, must not delete the `GENERAL PURPOSE` header to shorten the file, must not replace the `echo` lines with `out_*`, and must not give `./build.sh` a menu or a self-install verb.

## 5. Related artifacts

| Artifact | Role |
|----------|------|
| `docs/requirements/index.md` | Registry |
| `docs/requirements/requirement-cpp-build-sh.md` | Shebang, `.`, and the typed verbs |
| `docs/requirements/requirement-cpp-coding-style.md` | C++ comment blocks |
| `build.sh` | The functions this file names |
| `tests/test-cpp-builds.sh` | `TP-CPP-BUILD-SH-STYLE` |

## Terminologies

### Shell function naming convention

**Definition:** The shell function naming convention is the mandatory categorized-prefix system for POSIX shell functions: every function name uses a stable category prefix, snake_case after the prefix, and no bare names such as `main`, `help`, or `install`. User-facing commands stay short English words. Function names stay prefixed.

**Human daily-life explanation:** People still say a short word at the counter. The tool behind the counter keeps a drawer label. The word you type and the function name are not the same label.

**Daily-life example:** You type `build`. The script calls `build_link`. There is no function whose name is only `build`.

## 6. Status history

| Date | Status | Notes |
|------|--------|-------|
| 2026-11-10 | Active | `build.sh` functions use the `build_` prefix and the header in §2.2. `tests/test-cpp-builds.sh` printed `PASS cpp builds` and exited 0. `TP-CPP-BUILD-SH-STYLE` is pass. |
| 2026-11-10 | Active | The same id checks that `GENERAL PURPOSE:`, `CIAO PRINCIPLES APPLIED:`, and `Last reviewed:` sit in that order above each function. The suite printed `PASS cpp builds` and exited 0. |

**Last updated**: 2026-11-10
**Owner**: Build script coding style
