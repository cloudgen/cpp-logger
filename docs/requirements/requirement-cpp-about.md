**file**: docs/requirements/requirement-cpp-about.md
**Status**: Active
**Updated**: 2026-11-10
**Philosophy**: CIAO v2.10.2 / CIAO-Lite (Caution • Intentional • Anti-fragile • Over-engineered / Over-protect)

## 1. Purpose

This file owns the `cpp-logger about` page. The page answers three live facts for this run: which program this is, what this computer is, and where the program is running from. The page is three blocks: identity, then the host check, then the install location. It reads the environment and path existence. It does not install. It does not start other programs to look the facts up.

The host check is the quality-assurance record of this run: which compiler built the binary, which C++ standard it claims, and which machine is running it. The install-location block names the global bin, the local bin, and the path of this process. The host-check label order follows the HalloTui about page. This page has no star box and no text menu. The verb stays listed in `requirement-cpp-parameters.md`. Cache paths and persistence stay the directories in `requirement-cpp-logger-folder.md`. The install bins stay the paths in `requirement-cpp-global-installation.md` and `requirement-cpp-local-installation.md`. `logger_print_about()` in `src/main/cpp/logger.cpp` is the only builder.

### 1.1 Human-facing

**In one sentence:** `cpp-logger about` names this program, this computer, and the path it is running from, so a tester can read the machine without writing a log.

| Box | Meaning | Example |
|-----|---------|---------|
| You / this login | The person reading the page | `cpp-logger about` |
| The other role | `logger_print_about()` | It prints the three blocks and returns |
| Not this file | The flag list, the dated log line, and JSON log lines | `requirement-cpp-parameters.md`, `requirement-cpp-logger.md`, and `requirement-cpp-json.md` |

| Includes | Excludes |
|----------|----------|
| Identity lines, the host-check lines, and the install-location lines | A star box, a numbered menu, and a single installed banner |
| The domain sentence `No database. No configuration file.` | Python, conda, pyenv, and a remote version lookup |

| Surface | What you open | What for |
|---------|---------------|----------|
| `cpp-logger about` | The page on standard output | The three live facts |
| `help` | The verb list | The same command, described in one line |

| You do… | What it means | What you type |
|---------|---------------|---------------|
| Read the identity | Name, version, description, domain sentence, runtime tools, and the entry point | `cpp-logger about` |
| Read the host check | Compiler, C++ standard, operating system, architecture, and the running path. A missing value stays blank on its line | Same page |
| Read the install location | The global bin, the local bin, and the path of this process. A missing file stays `not found` on its line | Same page |

## 2. Core rules

1. `cpp-logger about` prints one page and exits 0. The page is three blocks in this order: identity, then host check, then install location. A blank line separates the blocks. The verb does not ask for a folder or a file. It does not append a log line. It exits before `component main start` and before `Starting cpp-logger`.
2. Quiet mode does not replace this page and does not hide it. `--json` and `JSON=1` print the one object in `requirement-cpp-json.md` and do not print this page beside it. The flag may sit before or after `about`. `--version` is not this page. `help` is not this page.
3. Building the page does not create the cache directory, the persistence directory, or a log directory. It does not call `sudo`, wrap `apt` or `dnf`, open a socket, or look up a remote version.

### 2.1 Identity

4. The first five lines are, in order: `{APP_NAME} {version}`, `{PROJECT_DESCRIPTION}`, `Domain: No database. No configuration file.`, `Runtime tools: none`, `Entry points: {APP_NAME}`. `{APP_NAME}` is `PROJECT_NAME` in `src/main/cpp/project.h` (`cpp-logger`). `{version}` is `VERSION_MAJOR`, `VERSION_MINOR`, and `VERSION_PATCH` from that header, printed as `1.1.3`. `{PROJECT_DESCRIPTION}` is the `PROJECT_DESCRIPTION` macro. This file owns the domain sentence. The version digits stay in `project.h`.
5. The runtime-tools line stays `none`. The page does not name FFmpeg, GTK, or WebKit.

### 2.2 Host check

6. The check starts with `{stamp} {APP_NAME}(v{version})  [CHECK SYSTEM]:`. There are two spaces before `[CHECK SYSTEM]:`. The next line is two spaces, then `Now checking your operation system!`.
7. `{stamp}` is local time `YYYY-MM-DD HH:MM:SS` plus the literal `.000000`. The page does not read a microsecond clock.
8. After the header, the page prints these labels in this order. Each line starts with four spaces, then the label, a colon, a space, and the value. A missing value leaves the line in place.

| Label | Read |
|-------|------|
| `Compiler` | The literal `g++`, a space, and the compiler macro `__VERSION__`. The page does not spawn `g++` |
| `C++ standard` | The literal `C++17` |
| `Operation System` | `uname` system name, a space, and `uname` release. The page uses the `uname` call. It does not start a shell |
| `Architecture` | `uname` machine, as returned. The page does not remap it |
| `Current User` | Environment `USER`, otherwise empty |
| `Shell` | Environment `SHELL`, otherwise empty |
| `Inside docker container` | `True` when `/.dockerenv` can be opened, otherwise `False` |
| `Location` | `readlink` of `/proc/self/exe`. When that fails, `cpp-logger` |
| `PID` | The process id, decimal |
| `Cache folder used` | Rule 9 |
| `Cache folder (preferred)` | Rule 9 |
| `Cache folder (1st fallback)` | Rule 9 |
| `Cache folder (2nd fallback)` | Empty. This product names no third cache directory. The line remains |
| `Persistence storage` | Rule 9. This read does not create it |
| `TTY / Interactive` | `yes` when stdout is a terminal, otherwise `no` |

9. **Cache and persistence.** These are the directories in `requirement-cpp-logger-folder.md`. The page reads them. It does not create them. Effective user id 0 is root or sudo. Every other user is the other environment.
    - **Root or sudo.** Preferred and used are `/var/cache/cpp-logger`. Persistence is the log directory `/var/cpp-logger/log` when that directory is the one `logger_persistence_storage()` returns. The 1st fallback is empty.
    - **Every other user.** Preferred is `/dev/shm/cache/cache-cpp-logger`. The 1st fallback is `$HOME/.cache/cache-cpp-logger` when `HOME` is set and non-empty, otherwise empty. Used is the preferred path when `/dev/shm` is a directory, otherwise the 1st fallback. Persistence is the log directory `$HOME/.app/cpp-logger/log` when `HOME` is set and non-empty and no earlier base was chosen. When `HOME` is missing or empty, and conda, pyenv, and `VIRTUAL_ENV` are unset, persistence is empty.
    - **Resolved log directory.** `logger_persistence_storage()` returns the log directory from `requirement-cpp-logger-folder.md`, or an empty string when that directory is unresolved. An explicit log directory is that path. A conda prefix, a pyenv virtualenv, or `VIRTUAL_ENV` yields `<that-prefix>/.app/cpp-logger/log`. The page prints that return. It does not print the base directory without `/log`, and it does not print a cache path in this field. `about` does not apply `--home` or `--prefix`.
    - **2nd fallback.** Empty on both environments. The leaf has no username and no process id.
10. The page does not print `Python`, `C Library`, `Python Executable`, `python2 location`, `python3 location`, `conda location`, `pyenv location`, `Cython String`, or `Binary Type`. It does not spawn `python2`, `python3`, `conda`, `pyenv`, `g++`, or `id`. It does not print `You are using the GLOBAL INSTALLED version`, `You are using the LOCAL INSTALLED version`, or `You are using an UNINSTALLED version`. It does not print a Repository row or an Install channel row.

### 2.3 Install location

11. A blank line follows the host check. The next line is `Install location`. The next three lines start with four spaces. Each names a path this process can be running from. The page reads path existence. It does not copy a binary and it does not execute the file at that path.
12. **Global install.** The path is `$PREFIX/bin/cpp-logger` when `PREFIX` is set and non-empty. Otherwise the path is `/usr/local/bin/cpp-logger`. The line is `Global install (<path>): <value>`. `<value>` is this build's version when that path is a regular file and is executable. Otherwise `<value>` is `not found`.
13. **Local install.** When `HOME` is set and non-empty, the path is `$HOME/.local/bin/cpp-logger` and the line is `Local install (<path>): <value>` with the same value rule. When `HOME` is missing or empty, the line is `Local install: not found`. The page does not invent a home path.
14. **Running from.** The value is the same read as `Location`: `readlink` of `/proc/self/exe`, or `cpp-logger` when that fails. The line is `Running from: <path>`.
15. Both probe lines stay. The page does not print `You are using the GLOBAL INSTALLED version`, `You are using the LOCAL INSTALLED version`, `You are using an UNINSTALLED version`, or `is properly installed`. It does not print a Repository row or an Install channel row.

### 2.4 Quality assurance

16. The three blocks are the quality-assurance record for this run. A tester reads which program this is, which compiler and computer are running it, and where that process is running from. An empty `HOME` leaves the home paths blank and leaves the local probe as `not found`. The lines stay. The check does not install a package and does not write the dated log file.
17. `TP-CPP-ABOUT` is that record on one binary. `TP-CPP-RUN-BOUNDARY` still requires the name line, the domain sentence, and the empty log directory.

### 2.5 What this page leaves on HalloTui

HalloTui prints a star box and classifies the path with three install-box sentences. It also opens the page from menu row 63. This page keeps two probe lines and the running path. It does not print those three sentences. There is no numbered menu, so row 83 is unclaimed.

| HalloTui page | This page |
|---------------|-----------|
| Identity, host check, and a star box | Identity, host check, and install location |
| Menu row 63 | The verb `about` |
| Three install-box sentences | Two probe lines and the running path |
| Cache leaves that include the user and the process id | `/dev/shm/cache/cache-cpp-logger`, `$HOME/.cache/cache-cpp-logger`, and `/var/cache/cpp-logger` |
| Domain sentence `Show a welcome message` | `No database. No configuration file.` |
| `CheckSystem` removed | No `CheckSystem` here either |

Identity block this file owns:

```text
cpp-logger 1.1.3
Console logger. Lines follow ChronicleLogger.
Domain: No database. No configuration file.
Runtime tools: none
Entry points: cpp-logger
```

Invocation. The verb does not ask for a path.

```text
cpp-logger about
```

## Under command line for normal user only

On Termux, Git Bash, Windows cmd, or the same class, the person runs `cpp-logger about` as the normal user. Building the about page does not use administrator privilege, `sudo`, `apt`, or a dedicated system user. Do not pipe a downloaded script into a shell.

## 3. Design principles (CIAO / CIAO-Lite)

- **Caution:** The host lines are the reads in `logger_print_about()`. The page does not invent a Python interpreter.
- **Intentional:** One page, three blocks, three live facts, one quality-assurance record.
- **Anti-fragile:** A missing home leaves the home paths empty and the local probe `not found`. It does not create them.
- **Over-protect:** No download command, no star box, no single installed banner, no `CheckSystem`.

## 4. Verification

| TP-ID | Pass |
|-------|------|
| TP-CPP-ABOUT | One binary. `about` prints the five identity lines, a blank line, the host-check labels in §2.2, a blank line, and the install-location lines in §2.3. The compiler line starts with `g++`. The standard line is `C++17`. The stamp ends `.000000` and two spaces sit before `[CHECK SYSTEM]:`. With `HOME` and `PREFIX` removed, the 1st fallback and persistence lines stay empty, the local probe is `Local install: not found`, and the global probe uses `/usr/local/bin/cpp-logger`. `Running from` is the running binary. With `VIRTUAL_ENV` set to `/tmp/cpp-logger-about-venv` and `HOME` removed, persistence is `/tmp/cpp-logger-about-venv/.app/cpp-logger/log` on the human page and in `persistence_storage`, and that directory is not created. The page contains no Python, conda, pyenv, star-box sentence, single installed banner, Repository row, or Install channel row. `--quiet about` still prints the host check and the install location. `--json about`, `about --json`, and `JSON=1 about` each print one JSON object and do not print the human page. The run creates no log directory and no cache directory |

### Design-time verification

| TP-ID | Suite | Status |
|-------|-------|--------|
| TP-CPP-ABOUT | `tests/test-cpp-about.sh` | pass |

**Matrix:** `docs/reviews/requirement-test-matrix.md`
**Map:** `docs/reviews/test-plan.md`

## 5. Protection rule

Future editors must not:

1. Add a star box, a single installed banner, a Repository row, or an Install channel row to this page. The two probe lines and the running path stay.
2. Print Python, conda, pyenv, `C Library`, `CheckSystem`, `curl -fsSL`, `GLOBAL INSTALLED`, `LOCAL INSTALLED`, `UNINSTALLED`, or `properly installed`.
3. Spawn `g++`, `python2`, `python3`, `conda`, `pyenv`, or `id` to fill a line, or execute the global or local bin to read its version.
4. Create the cache directory, the persistence directory, or a log directory while building the page.
5. Hide this page when quiet mode is on, or print this page beside the JSON object when `--json` or `JSON=1` is set.
6. Append a log line from `about`, or open the text menu from `about`.
7. Replace the folder-law cache paths with a process-id leaf.
8. Mark `TP-CPP-ABOUT` pass in `docs/reviews/test-plan.md` before `tests/test-cpp-about.sh` has exited 0.

**Violating this rule is a critical structure regression.**

## 6. Related artifacts

| Artifact | Role |
|----------|------|
| `docs/requirements/index.md` | Registry |
| `docs/requirements/requirement-cpp-parameters.md` | The verb `about` on the flag list |
| `docs/requirements/requirement-cpp-logger-folder.md` | Cache directories and persistence |
| `docs/requirements/requirement-cpp-global-installation.md` | Global bin the probe reads |
| `docs/requirements/requirement-cpp-local-installation.md` | Local bin the probe reads |
| `docs/requirements/requirement-cpp-logger.md` | `about` emits no `Starting` line |
| `docs/requirements/requirement-cpp-debug-mode.md` | `about` exits before the component pair |
| `docs/requirements/requirement-cpp-json.md` | The one object `--json` and `JSON=1` print instead of this page |
| `docs/requirements/requirement-cpp-tests.md` | `TP-CPP-ABOUT` |
| `docs/reviews/test-plan.md` | TP status |
| `docs/reviews/requirement-test-matrix.md` | This requirement to TP-CPP-ABOUT |
| `src/main/cpp/logger.cpp` | `logger_print_about()` |
| `src/main/cpp/project.h` | Name, version, and description |
| `tests/test-cpp-about.sh` | The three-block suite |
| `2026-11-10-checklist-cpp-about.md` | Filled run |

**Last updated**: 2026-11-10
**Owner**: About
