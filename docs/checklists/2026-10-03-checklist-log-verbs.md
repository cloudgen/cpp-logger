# Checklist: list and show

**Filled run** for `requirement-cpp-logger.md` and `requirement-cpp-tests.md` §2.2.

**Date:** 2026-10-03

## Warning

Do not call `mktemp` or `mkstemp`. Do not read, expand, assign, or delete `HOME`. If a new home assignment fails, the shell keeps the original home. `rm -rf` of that value deletes the user's files. This check builds one binary and uses the literal directory `/tmp`. It does not start a normal run. Cleanup deletes `/tmp/.app/cpp-logger`, `/tmp/var/cpp-logger`, `/tmp/cpp-logger-home-decoy`, and `/tmp/cpp-logger-prefix-decoy`, removes `/tmp/.app` and `/tmp/var` only when those directories are empty, and deletes `/tmp/cpp-logger-verb-work`. It does not delete `/tmp` and it does not delete a home directory.

## Checks

- [x] One `g++` binary at `/tmp/cpp-logger-verb-work/cpp-logger`. The check does not run `./build.sh`
- [x] `list --home /tmp` before any file exists prints no names and does not create `/tmp/.app/cpp-logger`
- [x] `list` without `--prefix` or `--home` exits 1 with `test purpose command requires --prefix or --home`. `show --home /tmp` without `--log-datestamp` exits 1 with `test purpose command requires --log-datestamp YYYYMMDD`
- [x] After two quiet `test` writes, `list --home /tmp` prints `cpp-logger-20261002.log` and then `cpp-logger-20261003.log`. `notes.txt` and a directory named `extra.log` are omitted
- [x] `show --home /tmp --log-datestamp 20261003` prints that file, including `test log`. `list` and `show` do not change the file
- [x] `--quiet` still prints the names. `show --json` prints the file and does not print a JSON object
- [x] A short `--log-datestamp` exits 1 with `--log-datestamp requires YYYYMMDD`. A missing stamp on `show` uses `test purpose command requires --log-datestamp YYYYMMDD`. `test list` exits 1
- [x] `list --prefix /tmp` is empty until a prefix file exists. Then it prints `cpp-logger-20261003.log`, and `show --prefix /tmp --log-datestamp 20261003` prints that file. `--prefix` wins over a different `--home`. `PREFIX` is ignored
- [x] `show --home /tmp --log-datestamp 20261004` exits 1 with `cannot read log file`. When the log path is a file, `list` exits 1 with `cannot read log directory`
- [x] The script does not expand `HOME` and does not invoke `mktemp` or `mkstemp`
- [x] `tests/test-log-verbs.sh` printed `PASS log verbs` on 2026-10-03

## Verdict

- [x] **Pass**
- [ ] **Revise**
- [ ] **Block**
