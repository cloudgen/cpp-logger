# Checklist: Logger class, five-argument constructor

**Filled run** for `requirement-cpp-oop.md` and `requirement-cpp-logger.md` §2.0 and §2.1.

**Date:** 2026-11-11

**Suite:** `tests/test-cpp-oop.sh` printed `PASS cpp oop` and exited 0. The script reads `logger.h`, `logger.cpp`, and `main.cpp`, compiles those units, `tests/cpp-logger-retain-driver.cpp`, and `tests/cpp-logger-format-driver.cpp` with `g++ -std=c++17 -Wall -Wextra -Werror`, and runs both drivers. The retain driver prints nothing and does not create a missing directory. The format driver checks an empty name, `CRITICAL`, a quiet create, and clock failure `19700101`. The script does not call `mktemp`. It does not read, assign, or delete `HOME`.

The 2026-11-10 checklist remains the record for `Logger logger(argc, argv);`. This file owns the five-argument constructor.

## Class

- [x] **TP-CPP-OOP** — `logger.h` declares `class Logger` once. `main.cpp` contains `Logger logger(PROJECT_NAME, logdir, basedir, quiet, NULL);` and does not declare a second type
- [x] The five-argument constructor is `Logger(const char *logname, const char *logdir, const char *basedir, bool is_quiet, LoggerClock clock)`. `Logger(int argc, char **argv)` is absent
- [x] `LOGGER_LEVEL_CRITICAL` is in the enumeration. `logger_critical` forwards to `logger_log`
- [x] An empty name creates no directory and prints nothing. A non-empty name prints `Created directory:` and the file starts with a newline
- [x] `CRITICAL` is on stderr and in the file. An empty component omits `@`. Clock failure writes `cpp-logger-19700101.log` and `[1970-01-01 00:00:00]`
- [x] A function whose only job is to return a `Logger` is absent. The `logger_*` names forward to that object

## Same-day suites on release 1.1.2

- [x] `tests/test-cpp-log-retention.sh` printed `PASS cpp log retention`. Readers left `20261001`. The following `test` log call removed it and left the active file plain
- [x] `tests/test-log-verbs.sh` printed `PASS log verbs`. `list` printed yesterday and today
- [x] `tests/test-logger-folder.sh` printed `PASS logger folder`. `VIRTUAL_ENV` wrote `.app/cpp-logger/log` under that prefix
- [x] `tests/test-run-boundary.sh` printed `PASS run boundary` for `cpp-logger 1.1.2`
- [x] `tests/test-cpp-logger-portable.sh` printed `PASS cpp logger portable`
- [x] `tests/test-cpp-about.sh` printed `PASS cpp about` for `cpp-logger 1.1.2`
- [x] `tests/test-cpp-builds.sh` printed `PASS cpp builds` after the 1.1.2 version strings

## Verdict

- [x] **Pass** — the five-argument constructor, the six levels, and the optional component exited 0 on 2026-11-11 at version 1.1.2. TP-CPP-OOP is pass
- [ ] **Revise**
- [ ] **Block**
