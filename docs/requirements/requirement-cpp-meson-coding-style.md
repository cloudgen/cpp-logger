**file**: docs/requirements/requirement-cpp-meson-coding-style.md
**Status**: Active
**Updated**: 2026-11-10
**Philosophy**: CIAO v2.10.2 / CIAO-Lite (Caution • Intentional • Anti-fragile • Over-engineered / Over-protect)

## 1. Purpose

This file owns the comment blocks, the verb names, and the no-hardcode rule in `meson.build`. Host, flags, dependencies, the debug path, `run`'s dependency, and the generated `project.sh` exports stay in `requirement-cpp-meson.md`. C++ comment blocks stay in `requirement-cpp-coding-style.md`. Ant banners stay in `requirement-cpp-ant-coding-style.md`. Gradle banners stay in `requirement-cpp-gradle-coding-style.md`.

### 1.1 Human-facing

**In one sentence:** You type a well-known Meson verb such as `meson compile -C target/exe/debug`, and every build statement in `meson.build` carries the same Purpose and Protection Rule banner.

| Box | Meaning | Example |
|-----|---------|---------|
| You / this login | The person who runs Meson | `meson introspect target/exe/debug`, then `meson compile -C target/exe/debug` |
| The other role | `meson.build` | Its commands keep the names in §2 |
| Not this file | Compile options and the output path | `requirement-cpp-meson.md` |

| Includes | Excludes |
|----------|----------|
| The well-known Meson verbs, the `#` comment block, and the rule that the binary name is the `PRJ` variable | C++ `/* */` function comments, Ant XML comments, and Gradle `//` banners |

| Surface | What you open | What for |
|---------|---------------|----------|
| `meson.build` | File header and statement banners | The block in §2.2 |
| `project.ini` | `PRJ` | The binary name |
| A shell | Meson on `PATH` | `meson <verb>` |

| You do… | What it means | What you type |
|---------|---------------|---------------|
| Build | The compile verb that writes the debug binary | `meson compile -C target/exe/debug` |
| See the project | Introspect lists the targets | `meson introspect target/exe/debug` |
| Launch the binary | `run` depends on the executable | `meson compile -C target/exe/debug --target run` |

## 2. Core rules

1. The typed form is `meson <verb>` with the `meson` binary on `PATH`. The setup directory is `target/exe/debug`. This file does not add a `meson` verb to `./build.sh`.
2. A well-known Meson verb keeps the spelling in §2.1. The script does not rename it.
3. The product target inside `meson.build` is `run`. Host tasks stay in `requirement-cpp-ant-coding-style.md` and `requirement-cpp-gradle-coding-style.md`. This file does not declare `build-windows`, `build-macos`, `buildWindows`, or `buildMac`.
4. The file opens with the header banner in §2.2. The header carries the do-not-simplify warning, `Purpose`, and `Critical CIAO Principles Applied`. The title line is `meson.build - CIAO DEFENSIVE EDITION`. It does not include the project name.
5. Every `dependency()`, `executable()`, and `run_target()` has the banner in §2.2 directly above it. The labels, in this order, are `Purpose`, `Parameters`, `Dependencies`, `Why This Design`, `CIAO Principles Applied`, and `Protection Rule`. That is the same label list as the Comment style section of `requirement-cpp-coding-style.md`, written as `#` line comments. A call with no extra parameter says `none` under `Parameters`.
6. Comments in this file stay `#` line comments inside the equals divider. They do not switch to the C++ `/* */` block, the Ant XML comment, or the Gradle `//` banner.
7. The binary name is the `PRJ` value read from `project.ini`. `executable()` and `run_target()` use that variable. A comment does not paste the product id as a literal. `project()` is the one place the fallback literal from `requirement-cpp-meson.md` appears, because Meson requires that call first. It does not appear again.
8. The version literal appears in `project()` once and matches `project.ini` `VERSION`. A later statement does not paste it again. The module names `glib-2.0` and `threads` appear once, on the `dependency()` calls. Their owner is `requirement-cpp-dependencies.md`. Source directories are variables. The debug binary path is the setup directory plus the `PRJ` variable.

### 2.1 Well-known Meson verbs

These are the commands an operator types. The Ant and Gradle peers are the same jobs in those files. What `compile` and `run` do to the binary is `requirement-cpp-meson.md`.

| Verb | Typed as | Ant peer | Gradle peer | What it is for |
|------|----------|----------|-------------|----------------|
| `setup` | `meson setup target/exe/debug` | `prepare` | configuration | Creates the build directory |
| `compile` | `meson compile -C target/exe/debug` | `build` | `assemble` | Builds the debug binary and does not launch it |
| `clean` | `meson compile -C target/exe/debug --clean` | `clean` | `clean` | Deletes generated build files in the setup directory |
| `test` | `meson test -C target/exe/debug` | | `check` | Verification. This file adds no `test()` body |
| `introspect` | `meson introspect target/exe/debug` | `ant -p` | `tasks` | Lists the project and its targets |
| `install` | `meson install -C target/exe/debug` | | | Copies the built binary into the Meson prefix. It does not run apt |
| `run` | `meson compile -C target/exe/debug --target run` | `run` | `run` | Depends on the executable, then executes the debug binary |

`meson compile` does not depend on a target that runs apt. `meson install` is not that apt command and is not the generator of `project.sh`.

### 2.2 Comment block

Divider line, the same equals rule as the Ant and Gradle banners, written as a Meson comment:

```text
# =========================================================================
```

File header. The title does not name the product.

```text
# =========================================================================
# meson.build - CIAO DEFENSIVE EDITION
# =========================================================================
#
# !!! THIS FILE IS PART OF THE CIAO DEFENSIVE PROGRAMMING STYLE !!!
# !!! DO NOT SIMPLIFY, REMOVE COMMENTS, OR "CLEAN UP" THIS FILE !!!
#
# Purpose:
#   Provides Meson build support for the C and C++ sources.
# Parameters:
#   none
# Dependencies:
#   PRJ from project.ini. executable() uses that value.
# Why This Design:
#   One file keeps the Meson build. The product id stays in project.ini.
# CIAO Principles Applied:
#   Intentional: every build statement has Purpose and Protection Rule.
# Protection Rule:
#   DO NOT hardcode the project name past the project() literal. DO NOT remove these comments.
# =========================================================================
```

Statement banner. `executable()` is the sample. `dependency()` and `run_target()` use the same labels and the same divider.

```text
# =========================================================================
# executable
#
# Purpose:
#   Builds the debug binary and does not launch it.
# Parameters:
#   none
# Dependencies:
#   dependency('glib-2.0') and dependency('threads') on Linux.
# Why This Design:
#   One executable is the build. run_target depends on it.
# CIAO Principles Applied:
#   Anti-fragile: the binary name is the PRJ value.
# Protection Rule:
#   DO NOT rename the run target. DO NOT hardcode the project name past project().
# =========================================================================
```

## 3. Design principles

- **Caution**: `meson install` stays a prefix copy. Setup does not call apt.
- **Intentional**: command names stay the spellings in §2.1. The product id stays in `project.ini` after the one `project()` literal.
- **Anti-fragile**: the banner labels match the C++ comment list, so a build statement is not left with only a name.

## 4. Verification

| TP-ID | Pass |
|-------|------|
| TP-CPP-MESON-STYLE | `meson.build` opens with the header banner and the title `meson.build - CIAO DEFENSIVE EDITION`. Each `dependency()`, `executable()`, and `run_target()` has the §2.2 labels in order. The verbs `setup`, `compile`, `clean`, `test`, `introspect`, `install`, and `run` keep those spellings. The product target is `run`. The file does not declare `build-windows`, `build-macos`, `buildWindows`, or `buildMac`. The product id literal appears only in `project()` |

### Design-time verification

| TP-ID | Suite | Status |
|-------|-------|--------|
| TP-CPP-MESON-STYLE | `tests/test-cpp-builds.sh` | pass |

**Matrix:** `docs/reviews/requirement-test-matrix.md`
**Map:** `docs/reviews/test-plan.md`

## 5. Protection rule

Future editors must not rename `compile` to `assemble` or `build`, must not delete the equals banners to shorten the file, must not hardcode the project name past the `project()` literal, must not move these comment rules into `requirement-cpp-coding-style.md`, and must not give `./build.sh` a `meson` verb. Binary path and host rules stay in `requirement-cpp-meson.md`.

## 6. Related artifacts

| Artifact | Role |
|----------|------|
| `docs/requirements/index.md` | Registry |
| `meson.build` | The banners and the command names |
| `project.ini` | `PRJ` |
| `docs/requirements/requirement-cpp-meson.md` | What `compile` and `run` do |
| `docs/requirements/requirement-cpp-ant-coding-style.md` | The Ant verb peers |
| `docs/requirements/requirement-cpp-gradle-coding-style.md` | The Gradle verb peers |
| `docs/requirements/requirement-cpp-coding-style.md` | The C++ label list this banner follows |
| `docs/reviews/test-plan.md` | TP-CPP-MESON-STYLE |
| `docs/reviews/requirement-test-matrix.md` | This requirement to TP-CPP-MESON-STYLE |
| `tests/test-cpp-builds.sh` | Static check for Ant, Gradle, and Meson |
| LM-CPP-MESON-CODING-STYLE | Law mold this file specializes |

**Last updated**: 2026-11-10
**Owner**: Meson coding style
