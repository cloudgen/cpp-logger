# Checklist: Ant, Gradle, Meson, and build.sh

**Filled run** for `requirement-cpp-ant.md`, `requirement-cpp-ant-coding-style.md`, `requirement-cpp-gradle.md`, `requirement-cpp-gradle-coding-style.md`, `requirement-cpp-meson.md`, `requirement-cpp-meson-coding-style.md`, `requirement-cpp-dependencies.md`, `requirement-cpp-build-sh.md`, and `requirement-cpp-build-sh-coding-style.md`.

**Date:** 2026-11-10

**Suite:** `tests/test-cpp-builds.sh` exited 0. The script reads `build.xml`, `build.gradle`, `meson.build`, `build.sh`, and `project.sh`. It runs `ant compile` and `gradle assemble --offline`. It does not run `./build.sh`, `meson`, or a package install. It does not call `mktemp`. It does not read, assign, or delete `HOME`. The child is started with `HOME` removed.

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

## Shell

- [x] **TP-CPP-BUILD-SH** — `build.sh` and `project.sh` start with `#!/bin/sh`. `build.sh` contains `. ./project.sh` and does not contain `source ./project.sh`. Neither file contains `[[`, `#!/bin/bash`, or `#!/usr/bin/env bash`. `build.sh` has a line `set -e`. Gradle runs the file with `sh`. The Ant generator writes `#!/bin/sh`. `sh -n` accepts both scripts
- [x] **TP-CPP-BUILD-SH-STYLE** — `build.sh` defines `build_install_deps`, `build_place_binary`, `build_install_local`, `build_install_global`, `build_clean`, and `build_link`. Above each function, `GENERAL PURPOSE:`, `CIAO PRINCIPLES APPLIED:`, and `Last reviewed:` appear in that order. Those three labels also head the file, so each appears at least seven times. No bare `build()`, `clean()`, `install()`, `main()`, `help()`, `install_deps()`, `install_local()`, `install_global()`, or `place_built_binary()`. The `case` keeps `install|deps`, `install-local`, `install-global`, `clean`, and `build|""|simple`. No `out_info`, `out_warn`, `out_die`, or `out_text`. `project.sh` still defines `apt_install`, `apt_uninstall`, and `install`

## Verdict

- [x] **Pass** — `tests/test-cpp-builds.sh` printed `PASS cpp builds` and exited 0 on 2026-11-10
- [ ] **Revise**
- [ ] **Block**
