**file**: docs/requirements/requirement-cpp-log-retention.md
**Status**: Active
**Updated**: 2026-11-10
**Philosophy**: CIAO v2.10.2 / CIAO-Lite (Caution • Intentional • Anti-fragile • Over-engineered / Over-protect)

## 1. Purpose

This file owns how long one dated log stays.

The intention is a bounded notebook. A recent day stays plain text, because that is the day a person still opens with `show`. An older day is still evidence, stored as one compressed package the host can open, so the plain copies do not accumulate. A day past the long window is no longer evidence this program keeps, so the plain file and the package both go. The write of today's line still succeeds when housekeeping fails.

ChronicleLogger states this intention: archive a day older than 7 days, and remove a day older than 30 days (`docs/ChronicleLogger-spec.md` in that sibling, and `LOG_ARCHIVE_DAYS` / `LOG_REMOVAL_DAYS` in its logger). Its spec calls those numbers environment settings. Its code keeps them as fixed class numbers and always writes `tar.gz`. This file keeps the two windows as 7 and 30. The package is the one that host opens: `zip` on Windows, `tar.gz` on every other host this product builds.

`requirement-cpp-logger.md` owns the line and the dated name. `requirement-cpp-logger-folder.md` owns the directory. This file owns the life of a day inside that directory.

### 1.1 Human-facing

**In one sentence:** A day stays plain for 7 days, becomes one compressed package after that, and is removed after 30 days.

| Box | Meaning | Example |
|-----|---------|---------|
| You / this login | The person who comes back to an old day | You open last week, then a day from last month |
| The other role | The writer that appends today's file | It compresses and removes other days in that directory |
| Not this file | The line shape, the directory, and `list` / `show` text | `requirement-cpp-logger.md` and `requirement-cpp-logger-folder.md` |

| Includes | Excludes |
|----------|----------|
| The 7-day plain window, the compressed day, and the 30-day removal | A second logger, a new verb, and a change to the line shape |

| Surface | What you open | What for |
|---------|---------------|----------|
| The log directory | Plain files and packages | See which days are still evidence |
| `show` | The plain file | Read a day that is still inside 7 days |

| You do… | What it means | What you see |
|---------|---------------|--------------|
| Read a recent day | The plain `cpp-logger-<YYYYMMDD>.log` is still there | `show` prints the lines |
| Read an older day | That day is one package, and the plain file is gone | The package sits beside the recent files |
| Come back after 30 days | That day is gone | The name is absent |

## 2. Core rules

1. The archive window is 7 days. The removal window is 30 days. The removal window is the longer one. A day moves from plain text, to one package, to gone.
2. The day is the eight digits in the file name, as a local date. The count is the number of days from that date to today's local date. Today is count 0. The comparison uses the name, so touching the file does not keep the day.
3. A name that is not `cpp-logger-<YYYYMMDD>.log` and not the package name in rule 5 stays in the directory.
4. Count 7 or less keeps the plain file. Retention does not require a package for that day.
5. Count greater than 7, and count 30 or less, keeps one package and removes the plain file after that package is complete. Windows writes `cpp-logger-<YYYYMMDD>.log.zip`. Every other host writes `cpp-logger-<YYYYMMDD>.log.tar.gz`. The package holds that one plain file.
6. Count greater than 30 removes the plain file and removes the package.
7. Today stays a plain file. The file this run is appending is the active file for that run. Retention leaves the active file plain for the rest of that run.
8. Retention runs when a normal run appends a line, in the log directory that run writes, and it applies to the other dated names. `help`, `about`, `list`, `show`, and `test` do not archive and do not delete. A missing log directory stays missing. Retention does not create it.
9. When the package cannot be written, the plain file stays and a partial package is removed. The line this run stored stays stored. Housekeeping does not change a successful write into a failed exit.
10. `list` lists plain `.log` names. `show` reads the plain file. A day that remains only as a package is absent from `list`, and `show` for that stamp exits 1 with `cannot read log file`. Those verbs do not unpack a package.

### Sample ages

Today is `2026-11-10`.

| Name date | Count | What remains |
|-----------|-------|----------------|
| `20261110` | 0 | `cpp-logger-20261110.log` |
| `20261103` | 7 | `cpp-logger-20261103.log` |
| `20261102` | 8 | `cpp-logger-20261102.log.tar.gz` on a non-Windows host, or `cpp-logger-20261102.log.zip` on Windows |
| `20261011` | 30 | The package for that day |
| `20261010` | 31 | Nothing for that day |

## 3. Verification

| TP-ID | Pass |
|-------|------|
| TP-CPP-LOG-RETENTION | `tests/test-cpp-log-retention.sh` exited 0 on 2026-11-10. The check plants names under literal directories in `/tmp`. It does not call `mktemp` or `mkstemp`. It does not read, assign, or delete `HOME`. A day at count 8 becomes the host package and the plain file is gone. A day at count 31 is gone. Today stays plain. A file whose name is not a dated log stays. `list` and `show` do not perform this maintenance |

### Design-time verification

| TP-ID | Suite | Status |
|-------|-------|--------|
| TP-CPP-LOG-RETENTION | `tests/test-cpp-log-retention.sh` | pass |

**Matrix:** `docs/reviews/requirement-test-matrix.md`
**Map:** `docs/reviews/test-plan.md`

`tests/test-cpp-log-retention.sh` proves this file. The `test`, `list`, and `show` checks in `requirement-cpp-tests.md` still choose a stamp and then read it. Their stamps may be older than 30 days on a later calendar day. That is why rule 8 keeps `test` out of retention.

## 4. Protection rule

Future editors must not:

1. Archive or delete the active file during the run that is appending it.
2. Run retention from `help`, `about`, `list`, `show`, or `test`.
3. Delete a file whose date cannot be read from a `cpp-logger-<YYYYMMDD>` name.
4. Remove the plain file when the package is incomplete.
5. Mark `TP-CPP-LOG-RETENTION` pass in `docs/reviews/test-plan.md` before a suite has exited 0.
6. Prove this rule by writing the login home, or by storing a scratch directory in `HOME`.

**Violating this rule is a critical structure regression.**

## 5. Related artifacts

| Artifact | Role |
|----------|------|
| `docs/requirements/index.md` | Registry |
| `docs/requirements/requirement-cpp-logger.md` | The line and the dated name |
| `docs/requirements/requirement-cpp-logger-portable.md` | The file prefix is the adopting project's name. This tree uses `cpp-logger` |
| `docs/requirements/requirement-cpp-logger-folder.md` | The directory these days live in |
| `docs/requirements/requirement-cpp-tests.md` | The checks that do not prove this row yet |
| `docs/reviews/test-plan.md` | TP status |
| `docs/reviews/requirement-test-matrix.md` | This requirement to TP-CPP-LOG-RETENTION |
| ChronicleLogger `LOG_ARCHIVE_DAYS` and `LOG_REMOVAL_DAYS` | The sibling windows, 7 and 30 |

**Last updated**: 2026-11-10
**Owner**: Logger
