# Checklist: portable logger name

**Filled run** for `requirement-cpp-logger-portable.md` and `requirement-cpp-tests.md` §2.6.

**Date:** 2026-11-10

**Suite:** `tests/test-cpp-logger-portable.sh` exited 0. The script builds one binary under `/tmp/cpp-logger-portable-work`. It does not call `mktemp` or `mkstemp`. It does not read, assign, or delete `HOME`. The child is started with `HOME` and `PREFIX` removed. It does not write a second project tree.

## Warning

Do not call `mktemp` or `mkstemp`. Do not read, expand, assign, or delete the home environment variable. Cleanup deletes `/tmp/.app/cpp-logger`, `/tmp/.app` when that directory is empty, and `/tmp/cpp-logger-portable-work`. It does not delete `/tmp` and it does not delete a home directory.

## Checks

- [x] `PRJ` in `project.ini` and `PROJECT_NAME` in `src/main/cpp/project.h` are the same name, `cpp-logger`
- [x] `VERSION` is `1.1.0`, the same triple as `VERSION_MAJOR`, `VERSION_MINOR`, and `VERSION_PATCH`
- [x] `LOG_COMPONENT` is `main`
- [x] `src/main/cpp/logger.cpp` does not paste `/var/cpp-logger`, `.app/cpp-logger`, `cache-cpp-logger`, or `/var/cache/cpp-logger`
- [x] `help` prints `Usage: cpp-logger`, `/var/cpp-logger/log`, `.app/cpp-logger/log`, and `cpp-logger-<YYYYMMDD>.log`
- [x] `test --home /tmp --log-datestamp 20261003` writes `/tmp/.app/cpp-logger/log/cpp-logger-20261003.log` and the line contains `@main`. It does not create `/tmp/var/cpp-logger`
- [x] `tests/test-cpp-logger-portable.sh` printed `PASS cpp logger portable` on 2026-11-10

## Verdict

- [x] **Pass**
- [ ] **Revise**
- [ ] **Block**
