# Checklist: Debug binary and payload download

**Filled run** for `requirement-cpp-debug-binary.md`, with the Ant, Gradle, and Meson rows that name that same file.

**Date:** 2026-11-11

**Release:** 1.2.1

**Suite:** `tests/test-cpp-builds.sh` exited 0. The script reads `build.sh`, `Makefile`, `build.xml`, `build.gradle`, `meson.build`, and `.gitignore`. It runs `ant compile`, `gradle assemble --offline`, and `make`. It does not run `meson` or `./build.sh`. It does not call `mktemp`. It does not read, assign, or delete `HOME`. The child is started with `HOME` removed.

## Live compile

- [x] `ant compile` exited 0 on 2026-11-11. After that run, `target/exe/debug/cpp-logger` and `download/cpp-logger` were the same bytes
- [x] `gradle assemble --offline` exited 0 on 2026-11-11. After the download file was removed, assemble wrote it again and `cmp` matched the debug binary
- [x] `make` exited 0 on 2026-11-11. After the download file was removed, Make wrote it again and `cmp` matched the debug binary
- [x] `./build.sh build` with `HOME` removed exited 0 on 2026-11-11. It printed `Download ready → download/cpp-logger`. `cmp` matched `target/exe/debug/cpp-logger`. The suite itself does not run `./build.sh`
- [x] `meson setup /tmp/cpp-logger-meson-debug-binary` and `meson compile -C /tmp/cpp-logger-meson-debug-binary` exited 0. `cmp` matched that binary with `download/cpp-logger`. The suite itself does not run `meson`
- [x] The suite printed `PASS cpp builds` and no `[FAIL]` line

## Debug binary

- [x] **TP-CPP-DEBUG-BINARY** — `build.sh`, `Makefile`, `build.xml`, `build.gradle`, and `meson.build` each name `target/exe/debug`, `download`, and `.exe`. `.gitignore` ignores `/download/`. Windows is `MINGW` in `build.sh`, `Windows_NT` in the Makefile, `exe.suffix` in `build.xml`, `osWindows` in `build.gradle`, and `is_windows` in `meson.build`

## Verdict

- [x] **Pass** — `tests/test-cpp-builds.sh` exited 0 on 2026-11-11 for release 1.2.1
- [ ] **Revise**
- [ ] **Block**
