# Checklist: cpp-log-path

**Filled run** for `requirement-cpp-logger.md`, `requirement-cpp-logger-folder.md`, and `requirement-cpp-tests.md` §2.1.

**Date:** 2026-10-03

## Warning

Do not call `mktemp` or `mkstemp`. Do not read, expand, assign, or delete `HOME`. If a new home assignment fails, the shell keeps the original home. `rm -rf` of that value deletes the user's files. This check builds one binary and uses the literal directory `/tmp`. It does not start a normal run. Cleanup deletes `/tmp/.app/cpp-logger` and `/tmp/var/cpp-logger`, removes `/tmp/.app` and `/tmp/var` only when those directories are empty, and deletes `/tmp/cpp-logger-folder-work`. It does not delete `/tmp` and it does not delete a home directory.

## Checks

- [x] One `g++` binary at `/tmp/cpp-logger-folder-work/cpp-logger`. The check does not run `./build.sh` and does not copy the binary into an install bin
- [x] `test` without `--prefix` or `--home` exits 1 with `test purpose command requires --prefix or --home` and creates no log file. `test --home /tmp` without a stamp exits 1 with `test purpose command requires --log-datestamp YYYYMMDD`. A stamp that is not eight digits exits 1 with `--log-datestamp requires YYYYMMDD`
- [x] `test --home /tmp --log-datestamp 20261003 --debug` without `--verbose` shows `switch --debug`, `component main start`, `component main end`, and `test log` on stdout and in the file. Re-run 2026-11-09.
- [x] The same command with `--verbose` prints `logger folder /tmp/.app/cpp-logger/log` and `log file /tmp/.app/cpp-logger/log/cpp-logger-20261003.log`
- [x] `--verbose --quiet` does not print a human log line. `--verbose --json` prints a JSON object and does not print the human line
- [x] That `--home` run does not create `/tmp/var/cpp-logger`
- [x] `test --prefix /tmp --log-datestamp 20261003 --debug --verbose` prints `logger folder /tmp/var/cpp-logger/log` and `log file /tmp/var/cpp-logger/log/cpp-logger-20261003.log`, and that file contains `test log`
- [x] `--prefix` wins over a different `--home`. The test command ignores `PREFIX`
- [x] The script does not expand `HOME` and does not invoke `mktemp` or `mkstemp`
- [x] `tests/test-logger-folder.sh` printed `PASS logger folder` on 2026-10-03

## Verdict

- [x] **Pass**
- [ ] **Revise**
- [ ] **Block**
