**file**: docs/requirements/requirement-cpp-tests.md
**Status**: Active
**Updated**: 2026-11-09
**Philosophy**: CIAO v2.10.2 / CIAO-Lite (Caution • Intentional • Anti-fragile • Over-engineered / Over-protect)

## 1. Purpose

This file owns how the program is checked. The named check is a full-program check.

### 1.1 Human-facing

**In one sentence:** You prove a C change with one compiled binary, then `test`, `list`, and `show`, and you read the dated log files.

| Box | Meaning | Example |
|-----|---------|---------|
| You / this login | The person who verifies | You run `tests/test-logger-folder.sh` and `tests/test-log-verbs.sh` |
| The other role | The test-plan map | It lists TP ids that are still todo |
| Not this file | The product rules those ids check | The owning requirement |

| Includes | Excludes |
|----------|----------|
| One test binary, the verbs `test`, `list`, and `show`, and the dated log file | Copying that binary into an install bin, and running `./build.sh` from these checks |

| Surface | What you open | What for |
|---------|---------------|----------|
| `./build.sh` | Compile check | The binary links |
| `docs/reviews/test-plan.md` | TP map | Status of each id |

| You do… | What it means | What you type |
|---------|---------------|---------------|
| Collect the other-user rehearsal file | One binary, then the test command | `cpp-logger test --home /tmp --log-datestamp 20261003` |
| Collect the root-or-sudo rehearsal file | The same binary | `cpp-logger test --prefix /tmp --log-datestamp 20261003` |
| Read the names and one file | The same binary, under the same switches | `cpp-logger list --home /tmp` and `cpp-logger show --home /tmp --log-datestamp 20261003` |

## 2. Core rules

Allowed:

1. `./build.sh build` compiles every `src/main/cpp/*.cpp` file. The log-file check compiles one binary with `g++` and does not run `./build.sh`, because sourcing `project.sh` installs apt packages.
2. A manual run with `DEBUG=1`, `--debug`, `--quiet`, `--json`, `help`, or the verbs `test`, `list`, and `show`.
3. The single-binary log-file check in §2.1.
4. The TP ids below. A TP id stays todo until that check has been run.

There is no GTK object and no WebKit object to test.

| TP-ID | Owner | Pass |
|-------|-------|------|
| TP-CPP-DEPENDENCIES | requirement-cpp-dependencies | `pkg-config --exists glib-2.0` succeeds, and both builds request that module and do not request GTK or WebKitGTK |
| TP-CPP-LOGGER-FOLDER | requirement-cpp-logger-folder and requirement-cpp-logger | §2.1. One binary. `test --home /tmp --log-datestamp 20261003` writes `/tmp/.app/cpp-logger/log/cpp-logger-20261003.log`. `test --prefix /tmp` with the same stamp writes `/tmp/var/cpp-logger/log/cpp-logger-20261003.log` |
| TP-CPP-LOG-VERBS | requirement-cpp-logger | §2.2. `list` prints the dated names. `show` prints one file. A missing switch, a short stamp, a second purpose verb, and a missing file each exit 1 |

### 2.1 Single-binary log-file check

A C check is one compiled binary and the cpp-test-purpose-command `test`. It is not a copy of that binary into an install bin. This check does not call `./build.sh`, `./build.sh install-local`, or `./build.sh install-global`. It does not start a normal run, because a normal run would use the login home or `/var/cpp-logger`.

**Warning.** A test must not call `mktemp` or `mkstemp`. A test must not read, expand, assign, or delete `HOME` in any form. If a test sets a new home and that assignment fails, the shell keeps the original home. `rm -rf` of that value then deletes the user's files. This check uses the literal directory `/tmp` only. Cleanup deletes `/tmp/.app/cpp-logger` and `/tmp/var/cpp-logger`, removes `/tmp/.app` and `/tmp/var` only when those directories are empty, and deletes `/tmp/cpp-logger-folder-work`. It does not delete `/tmp` and it does not delete a home directory.

1. Compile `src/main/cpp/logger.cpp` and `src/main/cpp/main.cpp` with `g++ -std=c++17 -Wall -Wextra -Werror`. Pass `pkg-config --cflags glib-2.0` before each `-c`. Link the two objects with `pkg-config --libs glib-2.0` and `-pthread`. The binary is `/tmp/cpp-logger-folder-work/cpp-logger`.
2. Run that binary as `test` with `HOME` and `PREFIX` removed from the process. Without `--prefix` or `--home`, the process exits 1 with `test purpose command requires --prefix or --home` and creates no log file. With `--home /tmp` and no `--log-datestamp`, it exits 1 with `test purpose command requires --log-datestamp YYYYMMDD` and creates no log file. A stamp that is not eight digits exits 1 with `--log-datestamp requires YYYYMMDD` and creates no log file.
3. Run `test --home /tmp --log-datestamp 20261003 --debug` on that same binary, still with `HOME` and `PREFIX` removed, and without `--verbose`. Stdout shows the human lines `switch --debug`, `component main start`, `component main end`, and `test log`. The file `/tmp/.app/cpp-logger/log/cpp-logger-20261003.log` contains those same messages. This run does not create `/tmp/var/cpp-logger`. It does not contain `Starting cpp-logger`.
4. Run the same command with `--verbose`. Stdout shows `logger folder /tmp/.app/cpp-logger/log` and `log file /tmp/.app/cpp-logger/log/cpp-logger-20261003.log`. `--verbose --quiet` does not show that human line. `--verbose --json` shows the JSON object and does not show the human line.
5. Run `test --prefix /tmp --log-datestamp 20261003 --debug --verbose` on that same binary. Stdout shows `logger folder /tmp/var/cpp-logger/log` and `log file /tmp/var/cpp-logger/log/cpp-logger-20261003.log`. That file contains `test log`. `--prefix` wins when both `--prefix /tmp` and a different `--home` are present. The test command ignores `PREFIX`. It does not read `HOME`.

The script is `tests/test-logger-folder.sh`. The filled checklist is `2026-10-03-checklist-logger-folder.md`.

### 2.2 Log list and show

`list` and `show` are test purpose commands on the same binary. This check is `tests/test-log-verbs.sh`. The filled checklist is `2026-10-03-checklist-log-verbs.md`. It uses the literal directory `/tmp` only. It does not start a normal run. Cleanup deletes `/tmp/.app/cpp-logger`, `/tmp/var/cpp-logger`, `/tmp/cpp-logger-home-decoy`, `/tmp/cpp-logger-prefix-decoy`, those parents when empty, and `/tmp/cpp-logger-verb-work`.

1. Compile one binary the same way as §2.1. The binary is `/tmp/cpp-logger-verb-work/cpp-logger`.
2. `list --home /tmp` with `HOME` and `PREFIX` removed prints no names and does not create `/tmp/.app/cpp-logger`.
3. `test --home /tmp --log-datestamp 20261002 --quiet` and `test --home /tmp --log-datestamp 20261003 --quiet` create two files. `list --home /tmp` prints `cpp-logger-20261002.log` and then `cpp-logger-20261003.log`. A `notes.txt` file and a directory named `extra.log` in that directory are omitted.
4. `show --home /tmp --log-datestamp 20261003` prints the contents of that file, including `test log`. `show` with a datestamp that has no file exits 1. `list` and `show` do not change the file. `--quiet` and `--json` still print the names and the file, and `show --json` does not print a JSON object.
5. `list --prefix /tmp` before a prefix file exists prints no names. After `test --prefix /tmp --log-datestamp 20261003 --quiet`, `list --prefix /tmp` prints `cpp-logger-20261003.log` and `show --prefix /tmp --log-datestamp 20261003` prints that file. `list` without a directory switch exits 1 with `test purpose command requires --prefix or --home`. A short `--log-datestamp` exits 1 with `--log-datestamp requires YYYYMMDD`. A missing stamp on `show` exits 1 with `test purpose command requires --log-datestamp YYYYMMDD`. `test list` exits 1. `--prefix` wins over a different `--home`. `PREFIX` is ignored. A missing file exits 1 with `cannot read log file`. When the log path is a file instead of a directory, `list` exits 1 with `cannot read log directory`.

### 2.3 Run boundary

This check is `tests/test-run-boundary.sh`. The filled checklist is `2026-11-09-checklist-run-boundary.md`. It uses the literal directory `/tmp` only. A normal run is started with `HOME` and `PREFIX` removed, so it does not write the login log file. Cleanup deletes `/tmp/.app/cpp-logger`, `/tmp/var/cpp-logger`, those parents when empty, and `/tmp/cpp-logger-boundary-work`.

1. Compile one binary the same way as §2.1. The binary is `/tmp/cpp-logger-boundary-work/cpp-logger`.
2. `about` exits 0 and prints `cpp-logger 1.0.8` and `No database. No configuration file.`. `help` prints `about`. Neither command creates `/tmp/.app/cpp-logger`.
3. A normal run with no flags prints no human log line. `--verbose` prints `Starting cpp-logger 1.0.8` and `Finished cpp-logger 1.0.8` and does not print `component main start`.
4. `--debug` prints `switch --debug`, `component main start`, `component main end`, `Starting cpp-logger 1.0.8`, and `Finished cpp-logger 1.0.8`. It does not print `switch --quiet`. `--debug --quiet` prints no human log line. `--debug --json` prints a JSON object whose message is `switch --debug` and does not print the human line.
5. `test --home /tmp --log-datestamp 20261003 --debug --quiet` stores `switch --debug`, `switch --quiet`, `switch --home /tmp`, `switch --log-datestamp 20261003`, `component main start`, `component main end`, and `test log`. It does not store `Starting cpp-logger`.

## 3. Protection rule

Future editors must not mark a TP id done in `docs/reviews/test-plan.md` until the check has been run. They must not invoke `mktemp` or `mkstemp` from `tests/test-logger-folder.sh`, `tests/test-log-verbs.sh`, or `tests/test-run-boundary.sh`, and they must not expand, assign, or delete `HOME` there.

## 4. Related artifacts

| Artifact | Role |
|----------|------|
| `docs/requirements/index.md` | Registry |
| `docs/reviews/test-plan.md` | TP status |
| `build.sh` | Compile check |
| `tests/test-logger-folder.sh` | One binary and the dated log file |
| `tests/test-log-verbs.sh` | `list` and `show` on that kind of binary |
| `tests/test-run-boundary.sh` | `about`, start, finish, and debug component lines |
| `2026-10-03-checklist-log-verbs.md` | Filled checklist for `list` and `show` |
| `docs/requirements/requirement-cpp-logger.md` | The file this check reads |
| `docs/requirements/requirement-cpp-logger-folder.md` | The directory this check prints |
| `2026-10-03-checklist-logger-folder.md` | Filled checklist for that check |
| `2026-11-09-checklist-run-boundary.md` | Filled checklist for the run boundary |

**Last updated**: 2026-11-09
**Owner**: Tests
