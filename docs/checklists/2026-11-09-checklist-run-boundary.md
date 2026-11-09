# Checklist: run boundary

**Filled run** for `requirement-cpp-logger.md`, `requirement-cpp-debug-mode.md`, `requirement-cpp-parameters.md`, and `requirement-cpp-tests.md` §2.3.

**Date:** 2026-11-09

## Warning

Do not call `mktemp` or `mkstemp`. Do not read, expand, assign, or delete `HOME`. This check builds one binary and uses the literal directory `/tmp`. A normal run is started with `HOME` and `PREFIX` removed. Cleanup deletes `/tmp/.app/cpp-logger`, `/tmp/var/cpp-logger`, those parents when empty, and `/tmp/cpp-logger-boundary-work`. It does not delete `/tmp` and it does not delete a home directory.

## Checks

- [x] One `g++` binary at `/tmp/cpp-logger-boundary-work/cpp-logger`. The check does not run `./build.sh`
- [x] `about` prints `cpp-logger 1.0.8` and `No database. No configuration file.` and does not create `/tmp/.app/cpp-logger`
- [x] `help` prints `about` and does not create that directory
- [x] A normal run with no flags prints no human log line
- [x] `--verbose` prints `Starting cpp-logger 1.0.8` and `Finished cpp-logger 1.0.8` and does not print `component main start`
- [x] `--debug` prints `switch --debug`, `component main start`, `component main end`, and the start and finished lines. It does not print `switch --quiet`
- [x] `--debug --quiet` prints no human log line. `--debug --json` prints the JSON object for `switch --debug` and does not print the human line
- [x] `test --home /tmp --log-datestamp 20261003 --debug --quiet` stores the switch lines, the component pair, and `test log`. It does not store `Starting cpp-logger`
- [x] `tests/test-run-boundary.sh` printed `PASS run boundary` on 2026-11-09

## Verdict

- [x] **Pass**
- [ ] **Revise**
- [ ] **Block**
