# Checklist: Logger class

**Filled run** for `requirement-cpp-oop.md`.

**Date:** 2026-11-10

**Suite:** `tests/test-cpp-oop.sh` exited 0. The script reads `logger.h`, `logger.cpp`, and `main.cpp`, compiles those units and `tests/cpp-logger-retain-driver.cpp` with `g++ -std=c++17 -Wall -Wextra -Werror`, and runs the retain driver on a directory that is not present. That run prints nothing. The script does not call `mktemp`. It does not read, assign, or delete `HOME`.

## Class

- [x] **TP-CPP-OOP** — `logger.h` declares `class Logger` once, with the members and methods collected in `requirement-cpp-oop.md` §2.3, including static `logger_active` and private static `logger_filename_cmp`
- [x] `logger.cpp` defines `Logger::Logger` and `Logger::` for each collected function. `va_copy` remains in `logger_log`
- [x] `main.cpp` contains `Logger logger(argc, argv);` and does not declare a second type
- [x] A function whose only job is to return a `Logger` is absent. The `logger_*` names forward to that object
- [x] Unbound `logger_maintain_directory` prints nothing and exits 0

## Existing suites on the same source

- [x] `tests/test-logger-folder.sh` printed `PASS logger folder`
- [x] `tests/test-log-verbs.sh` printed `PASS log verbs`
- [x] `tests/test-run-boundary.sh` printed `PASS run boundary`
- [x] `tests/test-cpp-about.sh` printed `PASS cpp about`
- [x] `tests/test-cpp-logger-portable.sh` printed `PASS cpp logger portable`
- [x] `tests/test-cpp-log-retention.sh` printed `PASS cpp log retention`
- [x] `tests/test-cpp-builds.sh` printed `PASS cpp builds`

## Verdict

- [x] **Pass** — `tests/test-cpp-oop.sh` printed `PASS cpp oop` and exited 0 on 2026-11-10
- [ ] **Revise**
- [ ] **Block**
