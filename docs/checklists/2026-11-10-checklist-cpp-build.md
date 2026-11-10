# Checklist: Ant, Gradle, and Meson

**Filled run** for `requirement-cpp-ant.md`, `requirement-cpp-ant-coding-style.md`, `requirement-cpp-gradle.md`, `requirement-cpp-gradle-coding-style.md`, `requirement-cpp-meson.md`, `requirement-cpp-meson-coding-style.md`, and `requirement-cpp-dependencies.md`.

**Date:** 2026-11-10

**Suite:** `tests/test-cpp-builds.sh` exited 0. The script reads the three build files and `build.sh`, then runs `ant compile` and `gradle assemble --offline`. It does not run `meson` or a package install. It does not call `mktemp`. It does not read, assign, or delete `HOME`. The child is started with `HOME` removed.

## Live compile

- [x] `ant compile` exited 0 on 2026-11-10. The log says `Check src/main/c: absent. Contributes no files.` and `Check src/main/cpp: present`. The run did not create `src/main/c` and did not change `project.sh`
- [x] `gradle assemble --offline` exited 0 on 2026-11-10. The log reports the same two lines
- [x] The suite printed no `[FAIL]` line for those two live checks

## Ant

- [x] **TP-CPP-ANT** — `build.xml` loads `PRJ` from `project.ini`, calls `pkg-config --cflags` and `pkg-config --libs` as separate commands, compiles with `-std=c++17 -Wall -Wextra -g -O2` and `-std=c11`, writes `target/exe/debug`, exports `SRC_C=$SRC/main/c` and `SRC_CPP=$SRC/main/cpp`, and `run` depends on `build`
- [x] **TP-CPP-ANT-STYLE** — the header title is `build.xml - CIAO DEFENSIVE EDITION`. The six banner labels are in order. The well-known targets set `description`. The `<project>` name attribute is not the product id

## Gradle

- [x] **TP-CPP-GRADLE** — `build.gradle` calls `pkg-config --cflags` and `pkg-config --libs` as separate commands, compiles with `-std=c++17 -Wall -Wextra -g -O2`, writes `target/exe/debug`, exports `SRC_C=$SRC/main/cpp` and `SRC_CPP=$SRC/main/cpp`, and `run` depends on `assemble`
- [x] **TP-CPP-GRADLE-STYLE** — the header title is `cpp-logger build.gradle`. The six banner labels are in order. The product tasks are `run`, `scriptVersion`, `installDependencies`, `buildWindows`, and `buildMac`

## Meson

- [x] **TP-CPP-MESON** — `meson.build` reads `project.ini`, names the Windows, macOS, and Linux libc host set, uses `c_std=c11`, `cpp_std=c++17`, `warning_level=2`, and `buildtype=debugoptimized`, names `target/exe/debug`, and has `run_target('run')` plus `SRC_C=$SRC/main/c` and `SRC_CPP=$SRC/main/cpp`
- [x] **TP-CPP-MESON-STYLE** — the header title is `meson.build - CIAO DEFENSIVE EDITION`. The six banner labels are in order. The product target is `run`

## Dependencies

- [x] **TP-CPP-DEPENDENCIES** — `pkg-config --exists glib-2.0` succeeded. `build.sh` and `meson.build` request `glib-2.0` and do not request `gtk+-3.0` or `webkit2gtk-4.1`

## Verdict

- [x] **Pass** — `tests/test-cpp-builds.sh` printed `PASS cpp builds` and exited 0 on 2026-11-10
- [ ] **Revise**
- [ ] **Block**
