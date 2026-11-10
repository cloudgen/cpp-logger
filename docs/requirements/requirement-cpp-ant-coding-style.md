**file**: docs/requirements/requirement-cpp-ant-coding-style.md
**Status**: Active
**Updated**: 2026-11-10
**Philosophy**: CIAO v2.10.2 / CIAO-Lite (Caution • Intentional • Anti-fragile • Over-engineered / Over-protect)

## 1. Purpose

This file owns the comment blocks, the verb names, and the no-hardcode rule in `build.xml`. Host, flags, pkg-config, the debug path, `run`'s dependency, and the generated `project.sh` exports stay in `requirement-cpp-ant.md`. C++ comment blocks stay in `requirement-cpp-coding-style.md`. Gradle banners stay in `requirement-cpp-gradle-coding-style.md`.

### 1.1 Human-facing

**In one sentence:** You type a well-known Ant verb such as `ant build`, and every target in `build.xml` carries the same Purpose and Protection Rule banner.

| Box | Meaning | Example |
|-----|---------|---------|
| You / this login | The person who runs Ant | `ant -p`, then `ant build` |
| The other role | `build.xml` | Its targets keep the names in §2 |
| Not this file | Compile flags and the output path | `requirement-cpp-ant.md` |

| Includes | Excludes |
|----------|----------|
| The well-known Ant verbs, the XML comment block, and the rule that the project name is `${app.name}` | C++ `/* */` function comments, and Gradle `//` banners |

| Surface | What you open | What for |
|---------|---------------|----------|
| `build.xml` | File header and target banners | The block in §2.2 |
| `project.ini` | `PRJ` | The only project name |
| A shell | Ant on `PATH` | `ant <verb>` |

| You do… | What it means | What you type |
|---------|---------------|---------------|
| Build | The default verb that writes the debug binary | `ant build` |
| See the names | Targets that set `description` are listed | `ant -p` |
| Launch the binary | `run` depends on `build` | `ant run` |

## 2. Core rules

1. The typed form is `ant <verb>` with the `ant` binary on `PATH`. The default target is `build`. This file does not add an `ant` verb to `./build.sh`.
2. A well-known Ant verb keeps the spelling in §2.1. The script does not rename it.
3. A target name is one lowercase word, or kebab-case when the name is more than one word. Host targets are `build-windows` and `build-macos`.
4. The file opens with the header banner in §2.2. The header carries the do-not-simplify warning, `Purpose`, and `Critical CIAO Principles Applied`. The title line is `build.xml - CIAO DEFENSIVE EDITION`. It does not include the project name.
5. Every `target` and every `macrodef` has the target banner in §2.2 directly above it. The labels, in this order, are `Purpose`, `Parameters`, `Dependencies`, `Why This Design`, `CIAO Principles Applied`, and `Protection Rule`. That is the same label list as the Comment style section of `requirement-cpp-coding-style.md`, written as one XML comment. A target with no parameter says `none` under `Parameters`. Each well-known target and each host target sets a `description`, so `ant -p` lists it.
6. Comments in this file stay XML comments inside the equals divider. They do not switch to the C++ `/* */` block or the Gradle `//` banner.
7. The project name is `${app.name}`, loaded from the `PRJ` key in `project.ini`. A target, an echo, a path, a generated line, and a comment do not paste the product id as a literal. The `<project>` element does not set `name` to the product id. The one fallback literal is the missing-key fallback in `requirement-cpp-ant.md`, and it appears only on that property default.
8. Version, author, apt package names, and the pkg-config module list are properties. A target does not paste them a second time. The module list's owner is `requirement-cpp-dependencies.md`. A path in a target uses `src.dir`, `src.c.dir`, `src.cpp.dir`, `build.dir`, or `build.debug.dir`. The debug binary is `${build.debug.dir}/${app.name}`.

### 2.1 Well-known Ant verbs

These names are the targets an operator types. What `build`, `compile`, `run`, and the host targets do to the binary is `requirement-cpp-ant.md`.

| Verb | Typed as | What it is for |
|------|----------|----------------|
| `build` | `ant build` | Default target. Builds the debug binary and does not launch it |
| `compile` | `ant compile` | Linux compile of `.c` and `.cpp` in one link on glibc, uClibc, or musl |
| `clean` | `ant clean` | Deletes generated build files |
| `install` | `ant install` | Generates `project.sh` when it is absent, then runs it |
| `run` | `ant run` | Depends on `build`, then executes the debug binary |
| `prepare` | `ant prepare` | Creates the debug directory |
| `make` | `ant make` | Autotools path: `autoreconf`, `configure`, and `make` |
| `build-windows` | `ant build-windows` | Windows target host. A Linux host skips this target |
| `build-macos` | `ant build-macos` | macOS target host. A Linux host skips this target |

`install` is not a dependency of `compile`, `build`, or `run`. `ant -p` lists each row because the target sets `description`.

### 2.2 Comment block

Divider line, the same equals rule as the Gradle banner, written as an XML comment:

```text
<!-- =========================================================================
```

File header. The title does not name the product.

```text
<!-- =========================================================================
     build.xml - CIAO DEFENSIVE EDITION
     =========================================================================

     !!! THIS FILE IS PART OF THE CIAO DEFENSIVE PROGRAMMING STYLE !!!
     !!! DO NOT SIMPLIFY, REMOVE COMMENTS, OR "CLEAN UP" THIS FILE !!!

     Purpose:
       Provides Ant build support for the C and C++ sources.
     Parameters:
       none
     Dependencies:
       PRJ from project.ini. Targets use ${app.name}.
     Why This Design:
       One file keeps the Ant targets. The product id stays in project.ini.
     CIAO Principles Applied:
       Intentional: every target has Purpose and Protection Rule.
     Protection Rule:
       DO NOT hardcode the project name. DO NOT remove these comments.
     ========================================================================= -->
```

Target banner. `build` is the sample. The other targets and each `macrodef` use the same labels and the same divider.

```text
<!-- =========================================================================
     build

     Purpose:
       Builds the debug binary and does not launch it.
     Parameters:
       none
     Dependencies:
       compile on Linux for glibc, uClibc, and musl. build-windows on Windows. build-macos on macOS.
     Why This Design:
       One target is the default. run depends on it.
     CIAO Principles Applied:
       Anti-fragile: the binary path is ${build.debug.dir}/${app.name}.
     Protection Rule:
       DO NOT rename this target. DO NOT hardcode the project name.
     ========================================================================= -->
```

## 3. Design principles

- **Caution**: `install` stays a verb the operator types. `compile`, `build`, and `run` do not depend on it.
- **Intentional**: target names stay the spellings in §2.1. The product id stays in `project.ini`.
- **Anti-fragile**: the banner labels match the C++ comment list, so a target is not left with only a name.

## 4. Verification

| TP-ID | Pass |
|-------|------|
| TP-CPP-ANT-STYLE | `build.xml` opens with the header banner and the title `build.xml - CIAO DEFENSIVE EDITION`. Each `target` and each `macrodef` has the §2.2 labels in order. The verbs `build`, `compile`, `clean`, `install`, `run`, `prepare`, `make`, `build-windows`, and `build-macos` keep those spellings and set `description`. The `<project>` element does not set `name` to the product id. A target uses `${app.name}` and does not paste the product id as a literal. The fallback literal appears only on the missing-key property default |

### Design-time verification

| TP-ID | Suite | Status |
|-------|-------|--------|
| TP-CPP-ANT-STYLE | `tests/test-cpp-builds.sh` | todo |

**Matrix:** `docs/reviews/requirement-test-matrix.md`
**Map:** `docs/reviews/test-plan.md`

## 5. Protection rule

Future editors must not rename `build` to another default word, must not delete the equals banners to shorten the file, must not hardcode the project name into a target or a comment, must not move these comment rules into `requirement-cpp-coding-style.md`, and must not give `./build.sh` an `ant` verb. Binary path and host rules stay in `requirement-cpp-ant.md`.

## 6. Related artifacts

| Artifact | Role |
|----------|------|
| `docs/requirements/index.md` | Registry |
| `build.xml` | The banners and the target names |
| `project.ini` | `PRJ` |
| `docs/requirements/requirement-cpp-ant.md` | What `build`, `compile`, `run`, and the host targets do |
| `docs/requirements/requirement-cpp-coding-style.md` | The C++ label list this banner follows |
| `docs/reviews/test-plan.md` | TP-CPP-ANT-STYLE |
| `docs/reviews/requirement-test-matrix.md` | This requirement to TP-CPP-ANT-STYLE |
| `tests/test-cpp-builds.sh` | Static check for Ant, Gradle, and Meson |
| LM-CPP-ANT-CODING-STYLE | Law mold this file specializes |

**Last updated**: 2026-11-10
**Owner**: Ant coding style
