# Checklist: Ant, Gradle, and Meson

**Filled run** for `requirement-cpp-ant.md`, `requirement-cpp-ant-coding-style.md`, `requirement-cpp-gradle.md`, `requirement-cpp-gradle-coding-style.md`, `requirement-cpp-meson.md`, and `requirement-cpp-meson-coding-style.md`.

**Date:** 2026-11-10

**Suite:** `tests/test-cpp-builds.sh` exited 1. The script reads the three build files, then runs `ant compile` and `gradle assemble --offline`. It does not run `meson` or a package install. It does not call `mktemp`. It does not read, assign, or delete `HOME`. The child is started with `HOME` removed.

## Live compile

- [x] `ant compile` exited 0 on 2026-11-10. The log says `Check src/main/c: absent. Contributes no files.` and `Check src/main/cpp: present`. The run did not create `src/main/c` and did not change `project.sh`
- [x] `gradle assemble --offline` exited 0 on 2026-11-10. The log reports the same two lines
- [x] The suite printed no `[FAIL]` line for those two live checks

## Ant

- [ ] **TP-CPP-ANT** — `build.xml` still combines `pkg-config --cflags` and `--libs` in one exec, omits `-Wextra -g -O2`, writes a path other than `target/exe/debug`, omits `SRC_CPP=$SRC/main/cpp`, and `run` does not depend on `build`
- [ ] **TP-CPP-ANT-STYLE** — the header title, the six banner labels, and `description` on `build`, `compile`, `clean`, `install`, `run`, `prepare`, and `make` are absent. The `<project>` name attribute is the product id

## Gradle

- [ ] **TP-CPP-GRADLE** — `build.gradle` calls `pkg-config --cflags --libs` as one command, compiles with a standard other than `-std=c++17`, omits `-Wall -Wextra -O2`, does not export `SRC_CPP=$SRC/main/cpp`, and `run` does not depend on `assemble`
- [ ] **TP-CPP-GRADLE-STYLE** — the six banner labels are absent

## Meson

- [ ] **TP-CPP-MESON** — `meson.build` does not read `project.ini`, does not name the Windows, macOS, and Linux libc host set, uses `warning_level=3`, omits `c_std=c11`, does not set the setup directory to `target/exe/debug`, and has no `run_target('run')` and no generated `SRC_C` / `SRC_CPP` pair
- [ ] **TP-CPP-MESON-STYLE** — the header title and the six banner labels are absent, and the product target is not `run`

## Verdict

- [ ] **Pass**
- [x] **Revise**
- [ ] **Block**
