**file**: docs/requirements/requirement-cpp-meson.md
**Status**: Active
**Updated**: 2026-11-10
**Philosophy**: CIAO v2.10.2 / CIAO-Lite (Caution • Intentional • Anti-fragile • Over-engineered / Over-protect)

## 1. Purpose

This file owns `meson.build`. It follows the host set, the C and C++ flags, the debug path, and the rule that `run` builds first in `requirement-cpp-ant.md` and `requirement-cpp-gradle.md`. The Meson spelling of those rules is this file. Comment banners and verb names are `requirement-cpp-meson-coding-style.md`. The primary build remains `./build.sh`, owned by `requirement-class-software-dev.md`. The link modules remain `requirement-cpp-dependencies.md`. The ship-unit directory remains `requirement-cpp-folder.md`.

### 1.1 Human-facing

**In one sentence:** Meson reads the binary name from `project.ini`, compiles C and C++ on Linux (glibc, uClibc, or musl), and writes `target/exe/debug/cpp-logger`.

| Box | Meaning | Example |
|-----|---------|---------|
| You / this login | The person who runs Meson | `meson setup target/exe/debug`, then `meson compile -C target/exe/debug` |
| The other role | `meson.build` | It loads `PRJ` and asks for `glib-2.0` |
| Not this file | Comment banners and the Meson verb names | `requirement-cpp-meson-coding-style.md` |

| Includes | Excludes |
|----------|----------|
| The Meson host set, `dependency()` for GLib and threads, the C and C++ options, the debug binary path, the `run` target, and the exports in a generated `project.sh` | Comment banners, verb spellings, GTK, WebKit, and a `meson` verb on `./build.sh` |

| Surface | What you open | What for |
|---------|---------------|----------|
| `project.ini` | `PRJ` | The binary name Meson uses |
| `meson.build` | `executable()` and `run_target()` | The Linux binary and the host gate |
| `target/exe/debug/` | `cpp-logger` | The debug executable |

| You do… | What it means | What you type |
|---------|---------------|---------------|
| Build with Meson | The Linux compile writes the debug binary | `meson compile -C target/exe/debug` |
| Launch that binary | `run` builds first | `meson compile -C target/exe/debug --target run` |

## 2. Core rules

1. The Meson binary name is the `PRJ` key in `project.ini`. `meson.build` reads that file after `project()`. `executable()` uses that key. When the file or the key is missing, the fallback name is `cpp-logger`. `project()` is the first call, so its name is that same fallback literal. The debug binary name and `PRJ` in a generated `project.sh` use the loaded key. Where `meson.build` writes that name is `requirement-cpp-meson-coding-style.md`. The script keeps no second project name.
2. The Meson target hosts are Windows, macOS, and Linux. A Linux target uses one libc family: glibc, uClibc, or musl. `host_machine.system()` of `windows` selects Windows. `host_machine.system()` of `darwin` selects macOS. `host_machine.system()` of `linux` selects Linux. On Linux the script records `glibc` when the compiler defines `__GLIBC__`, `uclibc` when it defines `__UCLIBC__`, and `musl` when it defines neither. glibc, uClibc, and musl all run the Linux compile. The script does not reject uClibc or musl. This is the same host set as `requirement-cpp-ant.md` and `requirement-cpp-gradle.md`. Meson does not add `build-windows`, `build-macos`, `buildWindows`, or `buildMac`.
3. On Linux, GLib and threads are `dependency('glib-2.0')` and `dependency('threads')`. That is the Meson form of the split `pkg-config --cflags` and `pkg-config --libs` calls in the Ant and Gradle files. Compile arguments from `glib-2.0` go on the compile. Link arguments from `glib-2.0` go on the link. `threads` adds `-pthread`, as `requirement-cpp-dependencies.md` requires. The module list stays that table. The file does not request `gtk+-3.0` or `webkit2gtk-4.1`.
4. On Linux, one `executable()` accepts `.c` and `.cpp` in the same link. It searches `src/main/c` and `src/main/cpp`. A missing directory contributes no files. The `project()` options are `c_std=c11`, `cpp_std=c++17`, `warning_level=2`, and `buildtype=debugoptimized`. `warning_level=2` is `-Wall -Wextra`. `buildtype=debugoptimized` is `-g -O2`. A `.c` file uses the C compiler. A `.cpp` file uses the C++ compiler. When any `.cpp` file is present, the linker is the C++ linker. The language level matches `requirement-class-software-dev.md`.
5. The setup directory is `target/exe/debug`. `meson setup target/exe/debug` then `meson compile -C target/exe/debug` writes `target/exe/debug/${PRJ}`. `PRJ` is the name from rule 1. That path is the same file `./build.sh` writes. Ant's `build` and Gradle's `assemble` are this compile. `meson compile` still only builds.
6. On Windows and on macOS, `meson.build` does not call `dependency('glib-2.0')` and does not call pkg-config. The Linux `executable()` is the compile for glibc, uClibc, and musl.
7. `run_target('run')` depends on the `executable()` from rule 4. `meson compile -C target/exe/debug --target run` builds that binary and then executes `target/exe/debug/${PRJ}` on Windows, on macOS, and on Linux for glibc, uClibc, and musl. This is the Meson form of Ant `run` depending on `build` and Gradle `run` depending on `assemble`.
8. Setup generates `project.sh` only when that file is absent. The generated file exports both `SRC_C` and `SRC_CPP`, the same pair as `requirement-cpp-ant.md`: `SRC_C` is `$SRC/main/c` and `SRC_CPP` is `$SRC/main/cpp`. The generated file also exports `PRJ` from rule 1. An existing `project.sh` stays as it is. The generator does not create `src/main/c` and does not run apt.

### 2.1 Implementation notes (this project)

`project.ini` sets `PRJ=cpp-logger`. The Linux debug binary is `target/exe/debug/cpp-logger`.

The ship-unit files are `src/main/cpp/logger.cpp` and `src/main/cpp/main.cpp`. Both take the C++17 options in rule 4. `requirement-cpp-folder.md` names no `.c` file. A `.c` file that is present under `src/main/c` or `src/main/cpp` joins that same link on the C11 option.

`project()` `version` is `1.1.2`, the same string as `project.ini` `VERSION` and `src/main/cpp/project.h`.

Generated exports, when `project.sh` is absent:

```text
export PRJ=cpp-logger
export SRC_C=$SRC/main/c
export SRC_CPP=$SRC/main/cpp
```

`SRC` remains `src` from `project.ini`.

### 2.2 Why this requirement exists

- **CIAO Principle 5** (https://github.com/cloudgen/ciao): one project name, read from `project.ini`, with the one `project()` literal Meson requires first.
- **CIAO Principle 1**: a missing GLib dependency fails at `dependency('glib-2.0')`, on the module name in `requirement-cpp-dependencies.md`.

## 3. Design principles

- **Caution**: Windows and macOS skip `dependency('glib-2.0')`. glibc, uClibc, and musl stay on that call. Setup does not run apt.
- **Intentional**: C and C++ are one `executable()`, with `c_std=c11`, `cpp_std=c++17`, `warning_level=2`, and `buildtype=debugoptimized`.
- **Anti-fragile**: `run` depends on the executable, so the binary matches the sources.

## 4. Verification

| TP-ID | Pass |
|-------|------|
| TP-CPP-MESON | `meson.build` reads `PRJ` from `project.ini` and the fallback is `cpp-logger`. The host gate is Windows, macOS, or Linux with glibc, uClibc, or musl. Linux uses `dependency('glib-2.0')` and `dependency('threads')` and does not request `gtk+-3.0` or `webkit2gtk-4.1`. Options are `c_std=c11`, `cpp_std=c++17`, `warning_level=2`, and `buildtype=debugoptimized`. A `.c` file and a `.cpp` file share one `executable()`. `meson compile -C target/exe/debug` writes `target/exe/debug/${PRJ}`. `run_target('run')` depends on that executable. The generator exports `SRC_C=$SRC/main/c` and `SRC_CPP=$SRC/main/cpp` |

### Design-time verification

| TP-ID | Suite | Status |
|-------|-------|--------|
| TP-CPP-MESON | `tests/test-cpp-builds.sh` | pass |

**Matrix:** `docs/reviews/requirement-test-matrix.md`
**Map:** `docs/reviews/test-plan.md`

## 5. Protection rule

Future editors must not drop Windows, macOS, glibc, uClibc, or musl from the host set, must not point Meson at GTK or WebKit, must not make setup run apt, and must not give `./build.sh` a `meson` verb. The module list stays in `requirement-cpp-dependencies.md`. The runtime name stays in `src/main/cpp/project.h`. The generator must not create `src/main/c`. Host target names from `requirement-cpp-ant.md` and `requirement-cpp-gradle.md` stay in those files.

## 6. Related artifacts

| Artifact | Role |
|----------|------|
| `docs/requirements/index.md` | Registry |
| `meson.build` | This law |
| `project.ini` | `PRJ` |
| `docs/requirements/requirement-cpp-ant.md` | The Ant host set, flags, path, and `run` this file follows |
| `docs/requirements/requirement-cpp-gradle.md` | The Gradle host set, flags, path, and `run` this file follows |
| `docs/requirements/requirement-cpp-dependencies.md` | `glib-2.0` and `-pthread` |
| `docs/requirements/requirement-cpp-folder.md` | Ship-unit files under `src/main/cpp` |
| `docs/requirements/requirement-class-software-dev.md` | Primary build tool and C++17 |
| `docs/requirements/requirement-cpp-meson-coding-style.md` | Comment banners and verb names |
| `docs/reviews/test-plan.md` | TP-CPP-MESON |
| `docs/reviews/requirement-test-matrix.md` | This requirement to TP-CPP-MESON |
| `tests/test-cpp-builds.sh` | Static check for Ant, Gradle, and Meson |
| LM-CPP-MESON | Law mold this file specializes |

**Last updated**: 2026-11-10
**Owner**: Meson
