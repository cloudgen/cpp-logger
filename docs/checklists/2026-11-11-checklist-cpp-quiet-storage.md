# Checklist: Quiet console, debug tokens, and log-folder persistence

**Filled run** for `requirement-cpp-output.md`, `requirement-cpp-debug-mode.md`, `requirement-cpp-json.md`, `requirement-cpp-about.md`, and `requirement-cpp-logger-folder.md`.

**Date:** 2026-11-11

**Release:** 1.1.3

The 2026-11-10 checklist remains the record for `Logger logger(argc, argv);`. The earlier 2026-11-11 class checklist remains the record for the five-argument constructor at 1.1.2. This file owns quiet, the debug tokens, and the log-directory return.

## Behavior

- [x] Quiet construction writes error, critical, and fatal into the dated file and prints no human console line
- [x] `logger_persistence_storage` returns the explicit log directory
- [x] `JSON=1` prints one error object, prints no human line, and still writes the human line
- [x] `DEBUG=yes` leaves debug off. `DEBUG=1`, `DEBUG=show`, `DEBUG=TRUE`, and `debug=1` turn debug on
- [x] `about` with `HOME` removed leaves Persistence storage empty and creates no log directory
- [x] `about` with `VIRTUAL_ENV=/tmp/cpp-logger-about-venv` prints `/tmp/cpp-logger-about-venv/.app/cpp-logger/log` and does not create that directory
- [x] A normal run still prints no human line until `--verbose` or debug mode. `--debug --quiet` prints no human line

## Suites

- [x] `tests/test-cpp-oop.sh` printed `PASS cpp oop` and exited 0
- [x] `tests/test-cpp-about.sh` printed `PASS cpp about` and exited 0 for `cpp-logger 1.1.3`
- [x] `tests/test-run-boundary.sh` printed `PASS run boundary` and exited 0
- [x] `tests/test-logger-folder.sh` printed `PASS logger folder` and exited 0
- [x] `tests/test-log-verbs.sh` printed `PASS log verbs` and exited 0
- [x] `tests/test-cpp-log-retention.sh` printed `PASS cpp log retention` and exited 0
- [x] `tests/test-cpp-logger-portable.sh` printed `PASS cpp logger portable` and exited 0
- [x] `tests/test-cpp-builds.sh` printed `PASS cpp builds` and exited 0

## Verdict

- [x] **Pass** — quiet, the debug tokens, JSON, and the log-folder return exited 0 on 2026-11-11 at version 1.1.3
- [ ] **Revise**
- [ ] **Block**
