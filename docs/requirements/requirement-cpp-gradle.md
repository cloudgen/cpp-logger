**file**: docs/requirements/requirement-cpp-gradle.md
**Status**: Active
**Updated**: 2026-11-10
**Philosophy**: CIAO v2.10.2 / CIAO-Lite (Caution • Intentional • Anti-fragile • Over-engineered / Over-protect)

## 1. Purpose

This file owns Gradle host, flags, pkg-config, the debug path, the `run` dependency, and the generated `project.sh` exports. Comment banners and verb names are `requirement-cpp-gradle-coding-style.md`. The primary build remains `./build.sh`, owned by `requirement-class-software-dev.md`. The link modules remain `requirement-cpp-dependencies.md`. The ship-unit directory remains `requirement-cpp-folder.md`.

### 1.1 Human-facing

**In one sentence:** Gradle reads the project name from `project.ini`, compiles C++17 on Linux (glibc, uClibc, or musl), and writes `target/exe/debug/cpp-logger`.

| Box | Meaning | Example |
|-----|---------|---------|
| You / this login | The person who runs Gradle | `gradle assemble`, then `gradle run` |
| The other role | `build.gradle` | It loads `PRJ` and calls pkg-config twice |
| Not this file | Comment banners and the Gradle verb names | `requirement-cpp-gradle-coding-style.md` |

| Includes | Excludes |
|----------|----------|
| The Gradle host set, the C++ flags, the split pkg-config calls, the debug binary path, the `run` task, and the exports in a generated `project.sh` | Comment banners, verb spellings, GTK, WebKit, and a `gradle` verb on `./build.sh` |

| Surface | What you open | What for |
|---------|---------------|----------|
| `project.ini` | `PRJ` | The project name Gradle uses |
| `build.gradle` | Compile, link, and `run` | The Linux binary and the Windows and macOS host gates |
| `target/exe/debug/` | `cpp-logger` | The debug executable |

| You do… | What it means | What you type |
|---------|---------------|---------------|
| Build with Gradle | The Linux compile writes the debug binary | `gradle assemble` |
| Launch that binary | `run` builds first | `gradle run` |

## 2. Core rules

1. The Gradle project name is the `PRJ` key in `project.ini`. `build.gradle` loads that file. When the file or the key is missing, the fallback name is `cpp-logger`. The header, the fallback, `scriptVersion`, the debug binary name, and `PRJ` in a generated `project.sh` use that name. The script keeps no second project name.
2. The Gradle target hosts are Windows, macOS, and Linux. A Linux target uses one libc family: glibc, uClibc, or musl. `os.name` containing `windows` selects Windows. `os.name` containing `mac` selects macOS. Every other `os.name` selects Linux. On Linux the script records `glibc`, `uclibc`, or `musl`. glibc, uClibc, and musl all run the Linux compile. The script does not reject uClibc or musl. Windows selects `buildWindows`. macOS selects `buildMac`.
3. On Linux, pkg-config is two commands. The module list is the table in `requirement-cpp-dependencies.md` (`glib-2.0`). The calls are `pkg-config --cflags` and `pkg-config --libs`. Compile receives the `--cflags` tokens. Link receives the `--libs` tokens. The script keeps every token from those two calls.
4. On Linux, each `CppCompile` task adds `-std=c++17`, `-Wall`, `-Wextra`, `-g`, and `-O2`. The language level matches `requirement-class-software-dev.md`. The script uses `-std=c++17` for that compile.
5. On Linux, the debug link writes `target/exe/debug/${PRJ}`. `PRJ` is the name from rule 1. `linkDebug` records that file. That path is the same file `./build.sh` writes. The link line also adds `-pthread`, as `requirement-cpp-dependencies.md` requires.
6. On Windows, the script skips `pkg-config`, and the `CppCompile` and `LinkExecutable` tasks do not run. `assemble` depends on `buildWindows`. `buildWindows` writes `target/exe/debug/${PRJ}`. It skips `pkg-config` and skips `gtk+-3.0` and `webkit2gtk-4.1`. On macOS, the script skips `pkg-config`, and those same compile and link tasks do not run. `assemble` depends on `buildMac`. `buildMac` writes `target/exe/debug/${PRJ}`. It skips `pkg-config` and skips `gtk+-3.0` and `webkit2gtk-4.1`.
7. The `run` task depends on `assemble` on Windows, on macOS, and on Linux for glibc, uClibc, and musl. Its executable is `target/exe/debug/${PRJ}`. `gradle assemble` still only builds.
8. `installDependencies` generates `project.sh` only when that file is absent. The generated file exports both `SRC_C` and `SRC_CPP`. Both values are `$SRC/main/cpp`, because `requirement-cpp-folder.md` names that directory as the only ship-unit source root. The generated file also exports `PRJ` from rule 1. An existing `project.sh` stays as it is.

### 2.1 Implementation notes (this project)

`project.ini` sets `PRJ=cpp-logger`. The Linux debug binary is `target/exe/debug/cpp-logger`.

Generated exports, when `project.sh` is absent:

```text
export PRJ=cpp-logger
export SRC_C=$SRC/main/cpp
export SRC_CPP=$SRC/main/cpp
```

`SRC` remains `src` from `project.ini`.

### 2.2 Why this requirement exists

- **CIAO Principle 5** (https://github.com/cloudgen/ciao): one project name, read from `project.ini`.
- **CIAO Principle 1**: a missing GLib flag fails at the split pkg-config calls, on the module name in `requirement-cpp-dependencies.md`.

## 3. Design principles

- **Caution**: Windows and macOS skip the Linux pkg-config line. glibc, uClibc, and musl stay on that line.
- **Intentional**: C++17 and the warning flags are written on the Gradle compile tasks, in the same spelling `./build.sh` uses.
- **Anti-fragile**: `run` depends on `assemble`, so the binary matches the sources.

## 4. Verification

| TP-ID | Pass |
|-------|------|
| TP-CPP-GRADLE | `build.gradle` loads `PRJ` from `project.ini` and the fallback is `cpp-logger`. The host gate is Windows, macOS, or Linux with glibc, uClibc, or musl. Linux calls `pkg-config --cflags` and `pkg-config --libs` as separate commands for `glib-2.0` and does not request `gtk+-3.0` or `webkit2gtk-4.1`. `CppCompile` adds `-std=c++17 -Wall -Wextra -g -O2`. `linkDebug` writes `target/exe/debug/${PRJ}`. `run` depends on `assemble`. The generator exports `SRC_C` and `SRC_CPP`, both `$SRC/main/cpp` |

### Design-time verification

| TP-ID | Suite | Status |
|-------|-------|--------|
| TP-CPP-GRADLE | `tests/test-cpp-builds.sh` | pass |

**Matrix:** `docs/reviews/requirement-test-matrix.md`
**Map:** `docs/reviews/test-plan.md`

## 5. Protection rule

Future editors must not drop Windows, macOS, glibc, uClibc, or musl from the host set, must not point Gradle at GTK or WebKit, must not make `run` depend on `installDependencies`, and must not give `./build.sh` a `gradle` verb. The module list stays in `requirement-cpp-dependencies.md`. The runtime name stays in `src/main/cpp/project.h`.

## 6. Related artifacts

| Artifact | Role |
|----------|------|
| `docs/requirements/index.md` | Registry |
| `build.gradle` | This law |
| `project.ini` | `PRJ` |
| `docs/requirements/requirement-cpp-dependencies.md` | `glib-2.0` and `-pthread` |
| `docs/requirements/requirement-cpp-folder.md` | `src/main/cpp` |
| `docs/requirements/requirement-class-software-dev.md` | Primary build tool and C++17 |
| `docs/requirements/requirement-cpp-gradle-coding-style.md` | Comment banners and verb names |
| `docs/reviews/test-plan.md` | TP-CPP-GRADLE |
| `docs/reviews/requirement-test-matrix.md` | This requirement to TP-CPP-GRADLE |
| `tests/test-cpp-builds.sh` | Live `gradle assemble` plus the Ant, Gradle, and Meson token checks |
| LM-CPP-GRADLE | Law mold this file specializes |

**Last updated**: 2026-11-10
**Owner**: Gradle
