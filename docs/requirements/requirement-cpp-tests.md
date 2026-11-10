**file**: docs/requirements/requirement-cpp-tests.md
**Status**: Active
**Updated**: 2026-11-10
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
| `./build.sh` | Interpreter and style checks | `tests/test-cpp-builds.sh` reads the script and does not run it |
| `docs/reviews/test-plan.md` | TP map | Status of each id |
| `tests/test-cpp-builds.sh` | Build files and `build.sh` | Ant, Gradle, Meson, and the shell entry |
| `tests/test-cpp-about.sh` | About page | Identity, the host check, and the install location |

| You do… | What it means | What you type |
|---------|---------------|---------------|
| Collect the other-user rehearsal file | One binary, then the test command | `cpp-logger test --home /tmp --log-datestamp 20261003` |
| Collect the root-or-sudo rehearsal file | The same binary | `cpp-logger test --prefix /tmp --log-datestamp 20261003` |
| Read the names and one file | The same binary, under the same switches | `cpp-logger list --home /tmp` and `cpp-logger show --home /tmp --log-datestamp 20261003` |

## 2. Core rules

Allowed:

1. `./build.sh build` compiles every `src/main/cpp/*.cpp` file. The log-file check compiles one binary with `g++` and does not run `./build.sh`, because reading `project.sh` with `.` runs its `install` function, which installs apt packages.
2. A manual run with `DEBUG=1`, `--debug`, `--quiet`, `--json`, `help`, or the verbs `test`, `list`, and `show`.
3. The single-binary log-file check in §2.1.
4. The TP ids below. A TP id stays todo until that check has been run.
5. §2.0. A check keeps writes off the login home by using a literal directory under `/tmp` and by removing `HOME` from the child. It does not store a scratch directory in `HOME`.

There is no GTK object and no WebKit object to test.

| TP-ID | Owner | Pass |
|-------|-------|------|
| TP-CPP-ANT | requirement-cpp-ant | `build.xml` loads `PRJ` from `project.ini` with fallback `cpp-logger`. Target hosts are Windows, macOS, and Linux with glibc, uClibc, or musl. Linux splits `pkg-config --cflags` and `pkg-config --libs` for `glib-2.0`. A `.cpp` file uses `-std=c++17 -Wall -Wextra -g -O2`. A `.c` file in the same link uses `-std=c11 -Wall -Wextra -g -O2`. The link writes `target/exe/debug/${PRJ}`. `run` depends on `build`. A generated `project.sh` exports `SRC_C` and `SRC_CPP` |
| TP-CPP-ANT-STYLE | requirement-cpp-ant-coding-style | The header banner titles `build.xml - CIAO DEFENSIVE EDITION`. Each target has `Purpose`, `Parameters`, `Dependencies`, `Why This Design`, `CIAO Principles Applied`, and `Protection Rule` in that order. Well-known verbs keep `build`, `compile`, `clean`, `install`, `run`, `prepare`, `make`, `build-windows`, and `build-macos`. The project name in a target is `${app.name}` |
| TP-CPP-BUILD-SH | requirement-cpp-build-sh | `build.sh` and `project.sh` start with `#!/bin/sh`. `build.sh` contains `. ./project.sh` and does not contain `source ./project.sh`. Neither file contains `[[`, `#!/bin/bash`, or `#!/usr/bin/env bash`. `build.sh` has a line `set -e`. Gradle runs `project.sh` with `sh`. The Ant generator writes `#!/bin/sh`. `sh -n` accepts both scripts |
| TP-CPP-BUILD-SH-STYLE | requirement-cpp-build-sh-coding-style | `build.sh` defines `build_install_deps`, `build_place_binary`, `build_install_local`, `build_install_global`, `build_clean`, and `build_link`. Above each function, `GENERAL PURPOSE:`, `CIAO PRINCIPLES APPLIED:`, and `Last reviewed:` appear in that order. Those three labels also head the file, so each appears at least seven times. No bare `build()`, `clean()`, `install()`, `main()`, `help()`, `install_deps()`, `install_local()`, `install_global()`, or `place_built_binary()`. The `case` keeps `install|deps`, `install-local`, `install-global`, `clean`, and `build|""|simple`. No `out_info`, `out_warn`, `out_die`, or `out_text`. `project.sh` still defines `apt_install`, `apt_uninstall`, and `install` |
| TP-CPP-DEPENDENCIES | requirement-cpp-dependencies | `pkg-config --exists glib-2.0` succeeds, and both builds request that module and do not request GTK or WebKitGTK |
| TP-CPP-GRADLE | requirement-cpp-gradle | `build.gradle` loads `PRJ` from `project.ini` with fallback `cpp-logger`. Target hosts are Windows, macOS, and Linux with glibc, uClibc, or musl. Linux splits `pkg-config --cflags` and `pkg-config --libs` for `glib-2.0`. `CppCompile` adds `-std=c++17 -Wall -Wextra -g -O2`. `linkDebug` writes `target/exe/debug/${PRJ}`. `run` depends on `assemble`. A generated `project.sh` exports `SRC_C` and `SRC_CPP` |
| TP-CPP-GRADLE-STYLE | requirement-cpp-gradle-coding-style | The header banner titles `cpp-logger build.gradle`. Each function and product task has `Purpose`, `Parameters`, `Dependencies`, `Why This Design`, `CIAO Principles Applied`, and `Protection Rule` in that order. Well-known verbs keep `help`, `tasks`, `properties`, `dependencies`, `clean`, `assemble`, `build`, and `check`. Product tasks are `run`, `scriptVersion`, `installDependencies`, `buildWindows`, and `buildMac` |
| TP-CPP-MESON | requirement-cpp-meson | `meson.build` reads `PRJ` from `project.ini` with fallback `cpp-logger`. Target hosts are Windows, macOS, and Linux with glibc, uClibc, or musl. Linux uses `dependency('glib-2.0')` and `dependency('threads')`. Options are `c_std=c11`, `cpp_std=c++17`, `warning_level=2`, and `buildtype=debugoptimized`. `meson compile -C target/exe/debug` writes `target/exe/debug/${PRJ}`. `run` depends on that executable. A generated `project.sh` exports `SRC_C` and `SRC_CPP` |
| TP-CPP-MESON-STYLE | requirement-cpp-meson-coding-style | The header banner titles `meson.build - CIAO DEFENSIVE EDITION`. Each `dependency()`, `executable()`, and `run_target()` has `Purpose`, `Parameters`, `Dependencies`, `Why This Design`, `CIAO Principles Applied`, and `Protection Rule` in that order. Well-known verbs keep `setup`, `compile`, `clean`, `test`, `introspect`, `install`, and `run`. The product id literal appears only in `project()` |
| TP-CPP-LOGGER-FOLDER | requirement-cpp-logger-folder and requirement-cpp-logger | §2.1. One binary. `test --home /tmp --log-datestamp 20261003` writes `/tmp/.app/cpp-logger/log/cpp-logger-20261003.log`. `test --prefix /tmp` with the same stamp writes `/tmp/var/cpp-logger/log/cpp-logger-20261003.log` |
| TP-CPP-LOG-VERBS | requirement-cpp-logger | §2.2. `list` prints the dated names. `show` prints one file. A missing switch, a short stamp, a second purpose verb, and a missing file each exit 1 |
| TP-CPP-ABOUT | requirement-cpp-about | §2.5. One binary. `about` prints the identity block, the host-check labels, and the install location. The page creates no log directory and no cache directory |
| TP-CPP-OOP | requirement-cpp-oop | §2.8. `logger.h` declares `class Logger`. `main.cpp` constructs it with the five arguments in `requirement-cpp-logger.md` §2.0 and declares no second type. The `logger_*` names forward to that object. The 2026-11-10 suite proved `Logger logger(argc, argv);` only. The 2026-11-11 suite proves the five-argument constructor |

### 2.0 Scratch paths and the login home

The intention is to keep a check from writing an install, a cache, or a persistence file into the login home.

`HOME` is the login home. These checks must not call `mktemp` or `mkstemp`, and they must not store a scratch directory in `HOME`. A one-command prefix applies only to that one command. After the next semicolon, `HOME` is the login home again. An export for the whole script makes the name of the login home mean a disposable directory.

The consequence is the cleanup. A recursive delete of `HOME` then deletes the login home: project trees, credential stores, and uncommitted work. The same delete hits the login home when the assignment fails and the shell keeps the original value.

These checks remove `HOME` from the child. Unset is not a scratch directory. Do not delete `HOME` afterward. Scratch paths are literal directories under `/tmp`. Cleanup deletes only those named paths. It does not delete `/tmp` and it does not delete a home directory.

### 2.1 Single-binary log-file check

A C check is one compiled binary and the cpp-test-purpose-command `test`. It is not a copy of that binary into an install bin. This check does not call `./build.sh`, `./build.sh install-local`, or `./build.sh install-global`. It does not start a normal run, because a normal run would use the login home or `/var/cpp-logger`.

**Warning.** This check follows §2.0. It uses the literal directory `/tmp` only. Cleanup deletes `/tmp/.app/cpp-logger` and `/tmp/var/cpp-logger`, removes `/tmp/.app` and `/tmp/var` only when those directories are empty, and deletes `/tmp/cpp-logger-folder-work`. It does not delete `/tmp` and it does not delete a home directory.

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
3. Two `test --home /tmp --quiet` runs use yesterday and today as `--log-datestamp`. `list --home /tmp` prints `cpp-logger-<yesterday>.log` and then `cpp-logger-<today>.log`. A `notes.txt` file and a directory named `extra.log` in that directory are omitted. Both stamps stay inside the 7-day window, so the second log call does not delete the first file.
4. `show --home /tmp` for today prints the contents of that file, including `test log`. `show` with datestamp `20261004` exits 1. `list` and `show` do not change the file. `--quiet` still prints the names and the file. `list --json` prints one object whose `files` array is yesterday then today. `show --json` prints one object whose `content` includes `test log` and whose text is not the raw file.
5. `list --prefix /tmp` before a prefix file exists prints no names. After `test --prefix /tmp` with today's stamp and `--quiet`, `list --prefix /tmp` prints `cpp-logger-<today>.log` and `show --prefix /tmp` for that stamp prints that file. `list` without a directory switch exits 1 with `test purpose command requires --prefix or --home`. A short `--log-datestamp` exits 1 with `--log-datestamp requires YYYYMMDD`. A missing stamp on `show` exits 1 with `test purpose command requires --log-datestamp YYYYMMDD`. `test list` exits 1. `--prefix` wins over a different `--home`. `PREFIX` is ignored. A missing file exits 1 with `cannot read log file`. When the log path is a file instead of a directory, `list` exits 1 with `cannot read log directory`.

### 2.3 Run boundary

This check is `tests/test-run-boundary.sh`. The filled checklist is `2026-11-09-checklist-run-boundary.md`. It uses the literal directory `/tmp` only. A normal run is started with `HOME` and `PREFIX` removed, so it does not write the login log file. Cleanup deletes `/tmp/.app/cpp-logger`, `/tmp/var/cpp-logger`, those parents when empty, and `/tmp/cpp-logger-boundary-work`.

1. Compile one binary the same way as §2.1. The binary is `/tmp/cpp-logger-boundary-work/cpp-logger`.
2. `about` exits 0 and prints `cpp-logger 1.1.2` and `No database. No configuration file.`. `help` prints `about`. Neither command creates `/tmp/.app/cpp-logger`.
3. A normal run with no flags prints no human log line. `--verbose` prints `Starting cpp-logger 1.1.2` and `Finished cpp-logger 1.1.2` and does not print `component main start`.
4. `--debug` prints `switch --debug`, `component main start`, `component main end`, `Starting cpp-logger 1.1.2`, and `Finished cpp-logger 1.1.2`. It does not print `switch --quiet`. `--debug --quiet` prints no human log line. `--debug --json` prints a JSON object whose message is `switch --debug` and does not print the human line.
5. `test --home /tmp --log-datestamp 20261003 --debug --quiet` stores `switch --debug`, `switch --quiet`, `switch --home /tmp`, `switch --log-datestamp 20261003`, `component main start`, `component main end`, and `test log`. It does not store `Starting cpp-logger`.

### 2.4 Ant, Gradle, Meson, and build.sh

This check is `tests/test-cpp-builds.sh`. The filled checklist is `2026-11-10-checklist-cpp-build.md`. It reads `build.xml`, `build.gradle`, `meson.build`, `build.sh`, and `project.sh`. It runs `ant compile` and `gradle assemble --offline`. It does not run `./build.sh`, `meson`, or a package install. It does not call `mktemp`. It does not read, assign, or delete `HOME`. The child is started with `HOME` removed. Logs are `/tmp/cpp-logger-ant-check.log` and `/tmp/cpp-logger-gradle-check.log`, and the script deletes those two files.

1. TP-CPP-ANT and TP-CPP-ANT-STYLE read `build.xml`. `ant compile` must exit 0. It must report `Check src/main/c: absent. Contributes no files.` and `Check src/main/cpp: present`. It must not create `src/main/c` and must not change `project.sh`.
2. TP-CPP-GRADLE and TP-CPP-GRADLE-STYLE read `build.gradle`. `gradle assemble --offline` must exit 0 and must report the same two check lines.
3. TP-CPP-MESON and TP-CPP-MESON-STYLE read `meson.build`.
4. TP-CPP-DEPENDENCIES runs `pkg-config --exists glib-2.0` and reads `build.sh` and `meson.build` for `glib-2.0`. Those files do not request `gtk+-3.0` or `webkit2gtk-4.1`.
5. TP-CPP-BUILD-SH reads the first line of `build.sh` and of `project.sh`. Both are `#!/bin/sh`. `build.sh` contains `. ./project.sh` and does not contain `source ./project.sh`. Neither file contains `[[`, `#!/bin/bash`, or `#!/usr/bin/env bash`. `build.sh` has a line `set -e`. `build.gradle` runs `project.sh` with `sh`. `build.xml` writes `#!/bin/sh`. `sh -n build.sh` and `sh -n project.sh` both succeed.
6. TP-CPP-BUILD-SH-STYLE reads `build.sh`. The functions are `build_install_deps`, `build_place_binary`, `build_install_local`, `build_install_global`, `build_clean`, and `build_link`. Above each function, `GENERAL PURPOSE:`, `CIAO PRINCIPLES APPLIED:`, and `Last reviewed:` appear in that order. Those three labels also head the file, so each appears at least seven times. A bare `build()`, `clean()`, `install()`, `main()`, `help()`, `install_deps()`, `install_local()`, `install_global()`, or `place_built_binary()` is absent. The `case` still has `install|deps`, `install-local`, `install-global`, `clean`, and `build|""|simple`. The script does not contain `out_info`, `out_warn`, `out_die`, or `out_text`. `project.sh` still defines `apt_install`, `apt_uninstall`, and `install`.
7. Each id stays todo until that id's checks in the script print no `[FAIL]` line. On 2026-11-10 the script printed `PASS cpp builds` and exited 0. TP-CPP-ANT, TP-CPP-ANT-STYLE, TP-CPP-GRADLE, TP-CPP-GRADLE-STYLE, TP-CPP-MESON, TP-CPP-MESON-STYLE, TP-CPP-DEPENDENCIES, TP-CPP-BUILD-SH, and TP-CPP-BUILD-SH-STYLE are pass.

### 2.5 About page

This check is `tests/test-cpp-about.sh`. The filled checklist is `2026-11-10-checklist-cpp-about.md`. It uses the literal directory `/tmp` only. The child is started with `HOME` and `PREFIX` removed. The script does not read, assign, or delete `HOME`. Cleanup deletes `/tmp/.app/cpp-logger`, `/tmp/.app` when that directory is empty, and `/tmp/cpp-logger-about-work`.

1. Compile one binary the same way as §2.1. The binary is `/tmp/cpp-logger-about-work/cpp-logger`. The check runs as a normal user.
2. `about` exits 0. The page starts with `cpp-logger 1.1.2`, the description, `Domain: No database. No configuration file.`, `Runtime tools: none`, and `Entry points: cpp-logger`. A blank line follows. The host check names `g++`, `C++17`, the `uname` system and machine, and the running binary. The stamp ends `.000000`. Two spaces sit before `[CHECK SYSTEM]:`. A blank line follows the host check. The next block is `Install location`.
3. With `HOME` removed, `Cache folder (1st fallback)`, `Cache folder (2nd fallback)`, and `Persistence storage` stay on the page and their values are empty. The preferred cache line is `/dev/shm/cache/cache-cpp-logger`. The local probe is `Local install: not found`. With `PREFIX` removed, the global probe uses `/usr/local/bin/cpp-logger` and the value is this build's version when that file is a regular executable, otherwise `not found`. `Running from` is the running binary. The run does not create the preferred cache directory and does not create `/tmp/.app/cpp-logger`.
4. The page does not contain Python, conda, pyenv, a star-box install sentence, `properly installed`, a Repository row, or an Install channel row. `--quiet about` still prints the host check and `Install location`. `--json about`, `about --json`, and `JSON=1` each print one JSON object and do not print the host check or `Install location`.

### 2.6 Logger portability

This check is `tests/test-cpp-logger-portable.sh`. It reads `PRJ` from `project.ini` and `PROJECT_NAME` from `src/main/cpp/project.h` and requires those strings to match. `VERSION` matches the triple from `VERSION_MAJOR`, `VERSION_MINOR`, and `VERSION_PATCH`. `LOG_COMPONENT` is `main`. Formats in `src/main/cpp/logger.cpp` do not paste `/var/<name>`, `.app/<name>`, or `cache-<name>`. `help` prints the name. `test --home /tmp --log-datestamp 20261003` writes `/tmp/.app/<name>/log/<name>-20261003.log`. The check does not build a second project. `TP-CPP-LOGGER-PORTABLE` is pass. The suite exited 0 on 2026-11-10 and is run again for release 1.1.2.

### 2.7 Log retention

This check is `tests/test-cpp-log-retention.sh`. It plants names under literal directories in `/tmp`. It does not call `mktemp` or `mkstemp`. It does not read, assign, or delete `HOME`. The driver freezes today at `20261110`. Count 7 stays a plain file. Count 8 is one `tar.gz` package and the plain file is gone. Count 30 is one package. Count 31 is gone. A name that is not a dated log stays. A failed `tar` leaves the plain file and leaves no partial package. `help`, `about`, `list`, and `show` leave `20261001` in place. A following `test --prefix` with stamp `20261103` removes `20261001` and leaves `20261103.log` as a plain file. `show` of a packaged day exits 1 with `cannot read log file`. `list` does not print a package. `TP-CPP-LOG-RETENTION` is pass. The suite exited 0 on 2026-11-10 for the frozen driver and is run again on 2026-11-11 for the `test` log call.

### 2.8 Logger class

This check is `tests/test-cpp-oop.sh`. It reads `src/main/cpp/logger.h`, `src/main/cpp/logger.cpp`, and `src/main/cpp/main.cpp`. It compiles those two units and `tests/cpp-logger-retain-driver.cpp` with `g++ -std=c++17 -Wall -Wextra -Werror`, then links the retain driver. It runs that driver on a directory that is not present. That run prints nothing and exits 0. The script does not call `mktemp` or `mkstemp`. It does not read, assign, or delete `HOME`. Objects stay under `/tmp/cpp-logger-oop-work`. Cleanup deletes that directory and `/tmp/cpp-logger-oop-absent`. It does not delete `/tmp`.

1. `logger.h` declares `class Logger` once, with the members and the methods collected in `requirement-cpp-oop.md` §2.3, including the static pointer `logger_active` and the private static method `logger_filename_cmp`.
2. `logger.cpp` defines `Logger::Logger` and `Logger::` for each collected function. `va_copy` stays in `logger_log`.
3. `main.cpp` parses `argc` and `argv`, then constructs `Logger` with the five arguments in `requirement-cpp-logger.md` §2.0, and does not declare a second type. The call is `Logger logger(PROJECT_NAME, logdir, basedir, quiet, NULL);`. The 1.1.1 call `Logger logger(argc, argv);` is not this check.
4. A function whose only job is to return a `Logger` is absent.
5. The `logger_*` C names forward to that object. Unbound `logger_maintain_directory` uses a short-lived object and does not print.

On 2026-11-10 the script printed `PASS cpp oop` and exited 0 for `Logger logger(argc, argv);`. That run is not this pass. On 2026-11-11 the same script proves the five-argument constructor. TP-CPP-OOP is pass for that constructor.

## 3. Protection rule

Future editors must not mark a TP id done in `docs/reviews/test-plan.md` until the check has been run. They must not call `mktemp` or `mkstemp` from `tests/test-logger-folder.sh`, `tests/test-log-verbs.sh`, `tests/test-run-boundary.sh`, `tests/test-cpp-builds.sh`, `tests/test-cpp-about.sh`, `tests/test-cpp-log-retention.sh`, `tests/test-cpp-logger-portable.sh`, or `tests/test-cpp-oop.sh`, and they must not read, expand, assign, or delete `HOME` there. §2.0 owns the intention and the consequence: a scratch directory stored in `HOME` does not stay bound past the next semicolon, and a later delete of `HOME` removes the login home.

## 4. Related artifacts

| Artifact | Role |
|----------|------|
| `docs/requirements/index.md` | Registry |
| `docs/reviews/test-plan.md` | TP status |
| `build.sh` | Compile check |
| `tests/test-logger-folder.sh` | One binary and the dated log file |
| `tests/test-log-verbs.sh` | `list` and `show` on that kind of binary |
| `tests/test-run-boundary.sh` | `about`, start, finish, and debug component lines |
| `tests/test-cpp-about.sh` | Identity, the host check, and the install location |
| `docs/requirements/requirement-cpp-about.md` | The page this check reads |
| `tests/test-cpp-log-retention.sh` | The 7-day package and the 30-day removal |
| `docs/requirements/requirement-cpp-log-retention.md` | The life of one dated day |
| `tests/test-cpp-logger-portable.sh` | One name from `project.ini` and `project.h` |
| `docs/requirements/requirement-cpp-logger-portable.md` | The same notebook for a later project's name |
| `2026-11-10-checklist-cpp-log-retention.md` | Filled checklist for retention |
| `2026-11-10-checklist-cpp-logger-portable.md` | Filled checklist for the one name |
| `2026-11-10-checklist-cpp-about.md` | Filled checklist for the about page |
| `2026-10-03-checklist-log-verbs.md` | Filled checklist for `list` and `show` |
| `docs/requirements/requirement-cpp-logger.md` | The file this check reads |
| `docs/requirements/requirement-cpp-logger-folder.md` | The directory this check prints |
| `docs/requirements/requirement-cpp-ant.md` | The Ant script this map names |
| `docs/requirements/requirement-cpp-ant-coding-style.md` | Ant banners, verb names, and the project-name spelling |
| `docs/requirements/requirement-cpp-meson.md` | The Meson build this map names |
| `docs/requirements/requirement-cpp-meson-coding-style.md` | Meson banners, verb names, and the `project()` literal |
| `docs/requirements/requirement-cpp-gradle.md` | The Gradle script this map names |
| `docs/requirements/requirement-cpp-gradle-coding-style.md` | Gradle banners and verb names |
| `2026-10-03-checklist-logger-folder.md` | Filled checklist for that check |
| `2026-11-09-checklist-run-boundary.md` | Filled checklist for the run boundary |
| `tests/test-cpp-oop.sh` | `class Logger`, the call-site construction, and the forwarding names |
| `2026-11-10-checklist-cpp-oop.md` | Filled checklist for the class |
| `docs/requirements/requirement-cpp-oop.md` | The class this check reads |
| `tests/test-cpp-builds.sh` | Ant, Gradle, and Meson build files |
| `2026-11-10-checklist-cpp-build.md` | Filled checklist for those three build files |
| `docs/reviews/requirement-test-matrix.md` | Requirement to TP map |

**Last updated**: 2026-11-11
**Owner**: Tests
