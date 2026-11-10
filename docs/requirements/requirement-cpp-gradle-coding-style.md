**file**: docs/requirements/requirement-cpp-gradle-coding-style.md
**Status**: Active
**Updated**: 2026-11-10
**Philosophy**: CIAO v2.10.2 / CIAO-Lite (Caution • Intentional • Anti-fragile • Over-engineered / Over-protect)

## 1. Purpose

This file owns the comment blocks and the verb names in `build.gradle`. Host, flags, pkg-config, the debug path, `run`'s dependency, and the generated `project.sh` exports stay in `requirement-cpp-gradle.md`. C++ comment blocks stay in `requirement-cpp-coding-style.md`.

### 1.1 Human-facing

**In one sentence:** You type a well-known Gradle verb such as `gradle assemble`, and every function and product task in `build.gradle` carries the same Purpose and Protection Rule banner.

| Box | Meaning | Example |
|-----|---------|---------|
| You / this login | The person who runs Gradle | `gradle tasks`, then `gradle assemble` |
| The other role | `build.gradle` | Its tasks keep the names in §2 |
| Not this file | Compile flags and the output path | `requirement-cpp-gradle.md` |

| Includes | Excludes |
|----------|----------|
| The well-known Gradle verbs, the product task names, and the `// ===` comment block | C++ `/* */` function comments (`requirement-cpp-coding-style.md`) |

| Surface | What you open | What for |
|---------|---------------|----------|
| `build.gradle` | File header and task banners | The block in §2.2 |
| A shell | Gradle on `PATH` | `gradle <verb>` |

| You do… | What it means | What you type |
|---------|---------------|---------------|
| Build | The lifecycle verb that writes the debug binary | `gradle assemble` |
| See the names | The help task lists product tasks beside the lifecycle verbs | `gradle tasks` |
| Print the script version | The product trace task | `gradle scriptVersion` |

## 2. Core rules

1. The typed form is `gradle <verb>` with the `gradle` binary on `PATH`. This file does not add a `gradle` verb to `./build.sh` and does not require `gradlew`.
2. A well-known Gradle verb keeps the spelling Gradle already ships. The script does not rename it and does not replace `help` or `tasks`.
3. A product task name is one lowercase word, or lower camel-case when the name is more than one word. The product names are `run`, `scriptVersion`, `installDependencies`, `buildWindows`, and `buildMac`.
4. The file opens with the header banner in §2.2. The title line names `cpp-logger build.gradle`. The header carries the do-not-simplify warning, `Purpose`, and `Critical CIAO Principles Applied`.
5. Every `def` function and every `task` or `tasks.register` has the task banner in §2.2 directly above it. The labels, in this order, are `Purpose`, `Parameters`, `Dependencies`, `Why This Design`, `CIAO Principles Applied`, and `Protection Rule`. That is the same label list as the Comment style section of `requirement-cpp-coding-style.md`, written as Groovy `//` lines. A task with no parameter says `none` under `Parameters`.
6. Comments in this file stay `//` line comments inside the equals divider. They do not switch to the C++ `/* */` block.

### 2.1 Well-known Gradle verbs

These names come with Gradle or with the `cpp-application` plugin. Behavior of `assemble` and of the Windows, macOS, and Linux libc host gates is `requirement-cpp-gradle.md`.

| Verb | Typed as | What it is for |
|------|----------|----------------|
| `help` | `gradle help` | Gradle help text |
| `tasks` | `gradle tasks` | Lists lifecycle verbs and the product tasks |
| `properties` | `gradle properties` | Project properties |
| `dependencies` | `gradle dependencies` | Dependency report |
| `clean` | `gradle clean` | Deletes the Gradle build output |
| `assemble` | `gradle assemble` | Builds the debug binary and does not launch it |
| `build` | `gradle build` | Lifecycle task. It is a different word from `assemble` |
| `check` | `gradle check` | Verification lifecycle. This file adds no check body |

### 2.2 Comment block

Divider line, copied from the banners already in `build.gradle`:

```text
// =========================================================================
```

File header:

```text
// =========================================================================
// cpp-logger build.gradle - CIAO DEFENSIVE EDITION
// =========================================================================
//
// !!! THIS FILE IS PART OF THE CIAO DEFENSIVE PROGRAMMING STYLE !!!
// !!! DO NOT SIMPLIFY, REMOVE COMMENTS, OR "CLEAN UP" THIS FILE !!!
//
// Purpose:
//   Provides Gradle build support using the cpp-application plugin.
//
// Critical CIAO Principles Applied:
//   - Every function/task has explicit Purpose and Protection Rule
// =========================================================================
```

Task banner. `scriptVersion` is the sample. The other product tasks and the helper functions use the same labels and the same divider.

```text
// =========================================================================
// scriptVersion
//
// Purpose:
//   Prints the Gradle script version and the project name.
// Parameters:
//   none
// Dependencies:
//   PRJ from project.ini, and the script version in this file.
// Why This Design:
//   One task prints the version so other tasks do not invent a second print.
// CIAO Principles Applied:
//   Intentional: the version string is visible before a build.
// Protection Rule:
//   DO NOT remove this task.
// =========================================================================
```

### 2.3 Product tasks

Each row is a task the script defines. What the task does to the binary, the host, or `project.sh` is `requirement-cpp-gradle.md`. This table owns the name, the group, and the description.

| Task | Group | Description |
|------|-------|-------------|
| `run` | `application` | Assemble the debug binary and execute `target/exe/debug/${PRJ}` |
| `scriptVersion` | `help` | Print the Gradle script version and the project name |
| `installDependencies` | `build` | Generate `project.sh` when it is absent, then run it |
| `buildWindows` | `build` | Windows target host. A Linux host skips this task |
| `buildMac` | `build` | macOS target host. A Linux host skips this task |

`installDependencies` is not a dependency of `assemble` or of `run`.

## 3. Design principles

- **Caution**: `installDependencies` stays a verb the operator types. `assemble` and `run` do not call it.
- **Intentional**: lifecycle names stay the Gradle spellings. Product names stay the tasks in §2.3.
- **Anti-fragile**: the banner labels match the C++ comment list, so a task is not left with only a name.

## 4. Verification

| TP-ID | Pass |
|-------|------|
| TP-CPP-GRADLE-STYLE | `build.gradle` opens with the header banner and the title `cpp-logger build.gradle`. Each `def` function and each product task has the §2.2 labels in order. The verbs `help`, `tasks`, `properties`, `dependencies`, `clean`, `assemble`, `build`, and `check` keep those spellings. The product tasks are `run`, `scriptVersion`, `installDependencies`, `buildWindows`, and `buildMac`, with the groups in §2.3. The file has no `gradlew` requirement |

### Design-time verification

| TP-ID | Suite | Status |
|-------|-------|--------|
| TP-CPP-GRADLE-STYLE | `tests/test-cpp-builds.sh` | pass |

**Matrix:** `docs/reviews/requirement-test-matrix.md`
**Map:** `docs/reviews/test-plan.md`

## 5. Protection rule

Future editors must not rename `assemble` to another build word, must not delete the equals banners to shorten the file, must not move these comment rules into `requirement-cpp-coding-style.md`, and must not give `./build.sh` a `gradle` verb. Binary path and host rules stay in `requirement-cpp-gradle.md`.

## 6. Related artifacts

| Artifact | Role |
|----------|------|
| `docs/requirements/index.md` | Registry |
| `build.gradle` | The banners and the task names |
| `docs/requirements/requirement-cpp-gradle.md` | What `assemble`, `run`, `buildWindows`, and `buildMac` do |
| `docs/requirements/requirement-cpp-coding-style.md` | The C++ label list this banner follows |
| `docs/reviews/test-plan.md` | TP-CPP-GRADLE-STYLE |
| `docs/reviews/requirement-test-matrix.md` | This requirement to TP-CPP-GRADLE-STYLE |
| `tests/test-cpp-builds.sh` | Static check for Ant, Gradle, and Meson |
| LM-CPP-GRADLE-CODING-STYLE | Law mold this file specializes |

**Last updated**: 2026-11-10
**Owner**: Gradle coding style
