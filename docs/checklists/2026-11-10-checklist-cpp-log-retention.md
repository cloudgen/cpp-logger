# Checklist: log retention

**Filled run** for `requirement-cpp-log-retention.md` and `requirement-cpp-tests.md` §2.7.

**Date:** 2026-11-10

**Suite:** `tests/test-cpp-log-retention.sh` exited 0. The script builds one binary and one driver under `/tmp/cpp-logger-retain-work`. It does not call `mktemp` or `mkstemp`. It does not read, assign, or delete `HOME`. The child is started with `HOME` and `PREFIX` removed. Today is frozen at `20261110`.

## Warning

Do not call `mktemp` or `mkstemp`. Do not read, expand, assign, or delete the home environment variable. Cleanup deletes `/tmp/cpp-logger-retain-work`, `/tmp/cpp-logger-retain-cli`, `/tmp/cpp-logger-retain-absent`, `/tmp/cpp-logger-retain-active`, `/tmp/cpp-logger-retain-fail`, and `/tmp/cpp-logger-retain-bin`. It does not delete `/tmp` and it does not delete a home directory.

## Checks

- [x] A missing log directory stays missing
- [x] An unreadable today date deletes nothing
- [x] Count 0 (`20261110`) stays the plain file. Count 7 (`20261103`) stays the plain file and gains no package
- [x] Count 8 (`20261102`) is `cpp-logger-20261102.log.tar.gz` and the plain file is gone. The package holds that one plain file. No `.zip` and no partial package remain
- [x] Count 30 (`20261011`) is one `tar.gz` package. Count 31 (`20261010`) is gone
- [x] `notes.txt`, `cpp-logger-2026.log`, and `cpp-logger-20260230.log` stay
- [x] The active plain file stays even when its date is past 30 days. A different day past 30 days is removed
- [x] A `tar` that exits 1 leaves the plain file and leaves no package and no partial package
- [x] After that, `test`, `list`, `show`, `help`, and `about` leave an older plain file in place. `list` does not print a package. `show` of the packaged day exits 1 with `cannot read log file`
- [x] `tests/test-cpp-log-retention.sh` printed `PASS cpp log retention` on 2026-11-10

## Verdict

- [x] **Pass**
- [ ] **Revise**
- [ ] **Block**
