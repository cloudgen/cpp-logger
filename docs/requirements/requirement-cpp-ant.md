**file**: docs/requirements/requirement-cpp-ant.md
**Status**: Active
**Updated**: 2026-11-11
**Philosophy**: CIAO v2.10.2 / CIAO-Lite (Caution • Intentional • Anti-fragile • Over-engineered / Over-protect)

## 1. Purpose

This file owns Ant host, flags, pkg-config, the debug path, the `run` dependency, and the generated `project.sh` exports. Comment banners, verb names, and the rule against a hardcoded project name are `requirement-cpp-ant-coding-style.md`. The primary build remains `./build.sh`, owned by `requirement-class-software-dev.md`. The link modules remain `requirement-cpp-dependencies.md`. The ship-unit directory remains `requirement-cpp-folder.md`. Gradle remains `requirement-cpp-gradle.md`.

### 1.1 Human-facing

**In one sentence:** Ant reads the project name from `project.ini`, compiles C and C++ together on Linux (glibc, uClibc, or musl), and writes the debug binary from `requirement-cpp-debug-binary.md`.

| Box | Meaning | Example |
|-----|---------|---------|
| You / this login | The person who runs Ant | `ant build`, then `ant run` |
| The other role | `build.xml` | It loads `PRJ` and calls pkg-config twice |
| Not this file | Comment banners, verb spellings, and a hardcoded project name | `requirement-cpp-ant-coding-style.md` |

| Includes | Excludes |
|----------|----------|
| The Ant host set, the split pkg-config calls, the C and C++ flags, the debug binary path, the `run` target, and the exports in a generated `project.sh` | Comment banners, verb spellings, a hardcoded project name, GTK, WebKit, libmpv, and an `ant` verb on `./build.sh` |

| Surface | What you open | What for |
|---------|---------------|----------|
| `project.ini` | `PRJ` | The project name Ant uses |
| `build.xml` | `compile`, `build`, and `run` | The Linux binary and the Windows, macOS, and libc host gates |
| `target/exe/debug/` | `cpp-logger` | The debug executable. The copy is `download/cpp-logger` |

| You do… | What it means | What you type |
|---------|---------------|---------------|
| Build with Ant | The Linux compile writes the debug binary | `ant build` |
| Launch that binary | `run` builds first | `ant run` |

## 2. Core rules

1. The Ant project name is the `PRJ` key in `project.ini`. `build.xml` loads that file. `app.name` is that key. When the file or the key is missing, the fallback name is `cpp-logger`. The debug binary name and `PRJ` in a generated `project.sh` use that name. Where `build.xml` writes that name is `requirement-cpp-ant-coding-style.md`. The script keeps no second project name.
2. The Ant target hosts are Windows, macOS, and Linux. A Linux target uses one libc family: glibc, uClibc, or musl. `os` family `windows` selects Windows. `os` family `mac` selects macOS. A Unix host that is not macOS selects Linux. On Linux the script records `glibc`, `uclibc`, or `musl`. glibc, uClibc, and musl all run `compile`. The script does not reject uClibc or musl. Windows sets `build-windows`. macOS sets `build-macos`. Windows and macOS skip `compile`.
3. On Linux, pkg-config is two commands. The module list is the table in `requirement-cpp-dependencies.md` (`glib-2.0`). The calls are `pkg-config --cflags` and `pkg-config --libs`. Each compile receives the `--cflags` tokens. The link receives the `--libs` tokens. The script keeps every token from those two calls.
4. On Linux, `compile` accepts `.c` and `.cpp` in the same link. It searches `src/main/c` and `src/main/cpp`. A missing directory contributes no files. A `.c` file is compiled with `gcc -std=c11 -Wall -Wextra -g -O2`. A `.cpp` file is compiled with `g++ -std=c++17 -Wall -Wextra -g -O2`. The language level matches `requirement-class-software-dev.md`. One source file is one compiler invocation. The object files go under `target/exe/debug/obj`.
5. On Linux, the link writes the debug binary from `requirement-cpp-debug-binary.md` and copies those bytes to the payload download file from that same requirement. `PRJ` is the name from rule 1. `./build.sh` writes that same file. When any `.cpp` file was compiled, the linker is `g++`. When every object is from a `.c` file, the linker is `gcc`. The link line also adds `-pthread`, as `requirement-cpp-dependencies.md` requires.
6. On Windows, `build` depends on `build-windows`. On macOS, `build` depends on `build-macos`. On Linux, for glibc, uClibc, and musl, `build` depends on `compile`. Each of those host targets uses the debug file and the download file from `requirement-cpp-debug-binary.md`. Windows and macOS name that file and do not write it. Windows and macOS skip `pkg-config`. `build` does not depend on `clean` or on `install`. `ant build` still only builds. `build` is the Ant target for the step Gradle calls `assemble`.
7. The `run` target depends on `build` on Windows, on macOS, and on Linux for glibc, uClibc, and musl. Its executable is the debug binary from `requirement-cpp-debug-binary.md`.
8. `install` generates `project.sh` only when that file is absent. The generated file starts with `#!/bin/sh`, which `requirement-cpp-build-sh.md` owns. It exports both `SRC_C` and `SRC_CPP`. `SRC_C` is `$SRC/main/c`. `SRC_CPP` is `$SRC/main/cpp`. The generated file also exports `PRJ` from rule 1. An existing `project.sh` stays as it is. The generator does not create `src/main/c`.

### 2.1 Implementation notes (this project)

`project.ini` sets `PRJ=cpp-logger`. The Linux debug binary is `target/exe/debug/cpp-logger`. The payload download file is `download/cpp-logger`. Windows appends `.exe` to both names. Those paths are `requirement-cpp-debug-binary.md`.

The ship-unit files are `src/main/cpp/logger.cpp` and `src/main/cpp/main.cpp`. Both take the C++17 line in rule 4. `requirement-cpp-folder.md` names no `.c` file. A `.c` file that is present under `src/main/c` or `src/main/cpp` joins that same link on the C11 line.

Generated exports, when `project.sh` is absent:

```text
export PRJ=cpp-logger
export SRC_C=$SRC/main/c
export SRC_CPP=$SRC/main/cpp
```

`SRC` remains `src` from `project.ini`.

### 2.2 Why this requirement exists

- **CIAO Principle 5** (https://github.com/cloudgen/ciao): one project name, read from `project.ini`.
- **CIAO Principle 1**: a missing GLib flag fails at the split pkg-config calls, on the module name in `requirement-cpp-dependencies.md`.

## 3. Design principles

- **Caution**: Windows and macOS skip the Linux pkg-config line. glibc, uClibc, and musl stay on that line. `compile` does not run `install`.
- **Intentional**: C and C++ are compiled in one link, each file with its own `-std` and the same `-Wall -Wextra -g -O2`.
- **Anti-fragile**: `run` depends on `build`, so the binary matches the sources.

## 4. Verification

| TP-ID | Pass |
|-------|------|
| TP-CPP-ANT | `build.xml` loads `PRJ` from `project.ini` and the fallback is `cpp-logger`. The host gate is Windows, macOS, or Linux with glibc, uClibc, or musl. Linux calls `pkg-config --cflags` and `pkg-config --libs` as separate commands for `glib-2.0` and does not request `gtk+-3.0` or `webkit2gtk-4.1`. A `.cpp` file is compiled with `-std=c++17 -Wall -Wextra -g -O2`. A `.c` file present beside it is compiled with `-std=c11 -Wall -Wextra -g -O2` and linked in the same binary. The link writes the debug binary from `requirement-cpp-debug-binary.md` and copies it to `download/`. On Windows the file name ends in `.exe`. `run` depends on `build`. The generator exports `SRC_C=$SRC/main/c` and `SRC_CPP=$SRC/main/cpp` |

### Design-time verification

| TP-ID | Suite | Status |
|-------|-------|--------|
| TP-CPP-ANT | `tests/test-cpp-builds.sh` | pass |

**Matrix:** `docs/reviews/requirement-test-matrix.md`
**Map:** `docs/reviews/test-plan.md`

## 5. Protection rule

Future editors must not drop Windows, macOS, glibc, uClibc, or musl from the host set, must not point Ant at GTK, WebKit, or libmpv, must not make `compile` or `run` depend on `install`, must not make `build` depend on `clean`, and must not give `./build.sh` an `ant` verb. The module list stays in `requirement-cpp-dependencies.md`. The runtime name stays in `src/main/cpp/project.h`. The generator must not create `src/main/c`.

## 6. Related artifacts

| Artifact | Role |
|----------|------|
| `docs/requirements/index.md` | Registry |
| `build.xml` | This law |
| `project.ini` | `PRJ` |
| `docs/requirements/requirement-cpp-dependencies.md` | `glib-2.0` and `-pthread` |
| `docs/requirements/requirement-cpp-folder.md` | Ship-unit files under `src/main/cpp` |
| `docs/requirements/requirement-class-software-dev.md` | Primary build tool and C++17 |
| `docs/requirements/requirement-cpp-ant-coding-style.md` | Comment banners, verb names, and the project-name spelling |
| `docs/reviews/test-plan.md` | TP-CPP-ANT |
| `docs/reviews/requirement-test-matrix.md` | This requirement to TP-CPP-ANT |
| `tests/test-cpp-builds.sh` | Live `ant compile` plus the Ant, Gradle, and Meson token checks |
| LM-CPP-ANT | Law mold this file specializes |

**Last updated**: 2026-11-10
**Owner**: Ant
