**file**: docs/requirements/requirement-cpp-build-sh.md
**Status**: Active
**Updated**: 2026-11-10
**Philosophy**: CIAO v2.10.2 / CIAO-Lite (Caution • Intentional • Anti-fragile • Over-engineered / Over-protect)

## 1. Purpose

This file owns the interpreter of `build.sh` and of `project.sh`. Without this file, the portable `/bin/sh` lesson arrives raw and the build stays on bash. The script is POSIX `/bin/sh` so the same file runs where bash is absent. It is the build entry. It is not a shell program that installs itself, prints a menu, or fetches a payload.

### 1.1 Human-facing

**In one sentence:** You run `./build.sh` under `/bin/sh`, and the script does not require bash.

| Box | Meaning | Example |
|-----|---------|---------|
| You / this login | The person who compiles | You run `./build.sh build` |
| The other role | `/bin/sh` | It reads `build.sh` and `project.sh` |
| Not this file | Which packages are installed, and where the binary is copied | `requirement-cpp-dependencies.md`, `requirement-cpp-local-installation.md`, and `requirement-cpp-global-installation.md` |

| Includes | Excludes |
|----------|----------|
| The `#!/bin/sh` line, `.` for reading `project.sh`, and the ban on bash-only syntax | A shell menu, a self-install channel, and a second copy of the C++ compile flags |

| Surface | What you open | What for |
|---------|---------------|----------|
| `build.sh` | The build entry | Compile, clean, and place |
| `project.sh` | The names the build reads | `PRJ` and the source directory |

| You do… | What it means | What you type |
|---------|---------------|---------------|
| Build | `/bin/sh` runs the script | `./build.sh build` |

## 2. Core rules

1. The first line of `build.sh` is `#!/bin/sh`. The first line of the checked-in `project.sh` is `#!/bin/sh`. Neither file starts with `#!/bin/bash` or `#!/usr/bin/env bash`.
2. `build.sh` reads `project.sh` with `. ./project.sh`. It does not use `source`. A missing `project.sh` uses the fallback names already in `build.sh`.
3. `build.sh` and `project.sh` stay inside the POSIX `/bin/sh` subset. They do not use `[[`, a shell array, process substitution, a here-string, or the `function` keyword. Command substitution with `$(...)` stays allowed. `local` inside a function stays allowed.
4. `build.sh` may keep `set -e` so a failed compile stops the script. This file does not replace those stops with a second printer family. The `[ERROR]` lines already in `build.sh` stay the human failure text.
5. The verbs stay `install`, `deps`, `install-local`, `install-global`, `clean`, `build`, `simple`, and an empty argument. `deps` is `install`. `simple` and an empty argument are `build`. This file does not add `make`, `ant`, `gradle`, or `meson`. The class file points here and still owns which tool is primary. A successful `build` writes the debug binary and the payload download copy named by `requirement-cpp-debug-binary.md`. `clean` removes that download copy.
6. `install` may run `sudo apt` for the packages in `requirement-cpp-dependencies.md`. `install-local` and `install-global` do not call `sudo`. Those two places stay in their own requirements.
7. Ant and Gradle write a missing `project.sh` with the first line `#!/bin/sh`. Gradle runs that file with `sh`. An existing `project.sh` stays as it is. The export lines stay in `requirement-cpp-ant.md`, `requirement-cpp-gradle.md`, and `requirement-cpp-meson.md`.
8. `sh -n build.sh` and `sh -n project.sh` both succeed. The proof is `tests/test-cpp-builds.sh`, id `TP-CPP-BUILD-SH`. Mark that id pass only after the script exits 0.

### 2.1 Implementation notes (this project)

| Item | Value |
|------|--------|
| Build entry | `build.sh` |
| Names file | `project.sh` |
| Interpreter | `/bin/sh` |
| Read | `. ./project.sh` |
| Product name | `PRJ=cpp-logger` in `project.sh` |
| Function style | `requirement-cpp-build-sh-coding-style.md` |
| Proof | `TP-CPP-BUILD-SH` in `tests/test-cpp-builds.sh` |

## 3. Design principles

- **Caution**: `sh -n` fails the check when the script is not valid `/bin/sh`.
- **Intentional**: one interpreter. Bash-only syntax is refused so a host without bash can still build.
- **Anti-fragile**: a missing `project.sh` keeps the fallback names. `set -e` stops on a failed compile.
- **Over-protect**: the shebang stays `#!/bin/sh`. The compile lines inside `build_link` stay where they are. The `build_` prefix is `requirement-cpp-build-sh-coding-style.md`.

## 4. Protection rule

Future editors must not put `#!/bin/bash` back on `build.sh` or `project.sh`, must not replace `. ./project.sh` with `source`, and must not turn this build entry into a menu or a self-install channel.

## 5. Related artifacts

| Artifact | Role |
|----------|------|
| `docs/requirements/index.md` | Registry |
| `docs/requirements/requirement-class-software-dev.md` | Primary build tool and the closed verb list |
| `docs/requirements/requirement-cpp-dependencies.md` | Apt packages for `install` |
| `docs/requirements/requirement-cpp-ant.md` | Generated `project.sh` exports |
| `docs/requirements/requirement-cpp-gradle.md` | Gradle runs `project.sh` with `sh` |
| `build.sh` | Build entry |
| `docs/requirements/requirement-cpp-build-sh-coding-style.md` | `build_` prefix and function headers |
| `project.sh` | Names the build reads |
| `tests/test-cpp-builds.sh` | `TP-CPP-BUILD-SH` |

## Terminologies

### Coding-style requirement

**Definition:** A coding-style requirement is the software-development specialize-in home for portable learned lessons about how the ship unit is written. Every software-development project must have an Active language-matched coding-style related requirement. Without that file, agents bring portable learned lessons raw and treat them as this product's law. The coding-style requirement is where those lessons are adopted, pointed at peer requirements, or refused.

**Human daily-life explanation:** A coding-style requirement is this product's adopted writing style home. Every software-dev project must have one. Without it, helpers would treat portable lessons as this product's law raw. The requirement is where those lessons are taken, pointed, or refused.

**Daily-life example:** The portable lesson is "the script starts with `#!/bin/sh`." This file adopts that lesson for `build.sh` and `project.sh`. It refuses the shell-program lessons that belong to a menu and a self-install channel.

## 6. Status history

| Date | Status | Notes |
|------|--------|-------|
| 2026-11-10 | Active | `build.sh` and `project.sh` are `#!/bin/sh`. `tests/test-cpp-builds.sh` printed `PASS cpp builds` and exited 0. `TP-CPP-BUILD-SH` is pass. |
| 2026-11-10 | Active | Function names point at `requirement-cpp-build-sh-coding-style.md`. The same suite run kept `TP-CPP-BUILD-SH` pass. |
| 2026-11-10 | Active | The same id also checks `set -e` and refuses `#!/usr/bin/env bash`. `tests/test-cpp-builds.sh` printed `PASS cpp builds` and exited 0. |

**Last updated**: 2026-11-10
**Owner**: Build script interpreter
