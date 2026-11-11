# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [1.2.1] - 2026-11-11

### Added
- The debug binary has one owner, `requirement-cpp-debug-binary.md`. Linux and macOS write `target/exe/debug/cpp-logger`. Windows writes `target/exe/debug/cpp-logger.exe`. `./build.sh`, GNU Make, Ant, Gradle, and Meson write that file.
- A successful link copies those same bytes to `download/cpp-logger` (`.exe` on Windows) for payload download. `download/` is generated output and stays out of git.

### Changed
- `project.h`, `project.ini`, `meson.build`, `build.gradle`, `build.xml`, and the README Version badge say 1.2.1.

## [1.2.0] - 2026-11-11

### Added
- `class Invocation` keeps the page, the purpose, and completion. `Invocation`, `AboutCli`, and `Help` each bind `Logger`. Under debug mode, a public call and a major member change are debug lines.

### Changed
- `help`, `about`, `list`, and `show` keep a debug line on the console and do not create a log directory. JSON or quiet on those commands leaves the console free of that line. JSON on those commands stays one object.
- `About` is the runtime source of the name, the version, the paths, and the flags. `Logger` binds that object. `main` constructs `About` and calls `run`. The about page is `AboutCli`. It prints the application directory as Persistence storage and that directory plus `/log` as the log folder. It does not print the log-directory return and it does not create either directory.
- GNU Make compiles every `.cpp` file in `src/main/cpp`.
- `project.h`, `project.ini`, `meson.build`, `build.gradle`, `build.xml`, and the README Version badge say 1.2.0.

## [1.1.3] - 2026-11-11

### Changed
- Quiet hides every human console line, including error, critical, and fatal. The dated file still receives the line. `--verbose` and debug do not override quiet.
- Debug mode is `--debug`, or `DEBUG` or `debug` set to `show`, `true`, or `1`. A different non-empty value does not enable debug.
- `logger_persistence_storage` returns the log directory. The about page prints that directory as Persistence storage and does not create it.
- JSON mode stays this program's machine channel. An error event is still one JSON object, and the human line stays off.
- `project.h`, `project.ini`, `meson.build`, `build.gradle`, `build.xml`, and the README Version badge say 1.1.3.

## [1.1.2] - 2026-11-11

### Changed
- `main` parses the command, then constructs `Logger` with the log name, the log directory, the base directory, quiet, and a clock. An empty name does not create a directory. A non-empty name resolves the base, creates the log directory, and writes one newline.
- The level enumeration includes `CRITICAL`. `logger_critical` writes that level. Error, critical, and fatal use stderr. The caller passes the component. An empty component omits `@`.
- The base directory is an explicit base, else the conda prefix, else the pyenv virtualenv, else `VIRTUAL_ENV`, else `/var/<name>` for effective user id 0, else `$HOME/.app/<name>`.
- A log call whose dated path changes runs the 7-day package and the 30-day removal before the append, including `test`. `help`, `about`, `list`, and `show` do not create or change the log directory.
- `build.sh` and `project.sh` start with `#!/bin/sh`. `build.sh` loads `project.sh` with `. ./project.sh`. Functions in `build.sh` use the `build_` prefix.
- `project.h`, `project.ini`, `meson.build`, `build.gradle`, `build.xml`, and the README Version badge say 1.1.2.

## [1.1.1] - 2026-11-10

### Added
- `tests/test-cpp-oop.sh` checks `class Logger`, the construction in `main`, and the `logger_*` names that forward to that object.

### Changed
- The logger is `class Logger`. `main` constructs `Logger logger(argc, argv)`. The `logger_*` names forward to that object. Log lines, retention, JSON, and the about page stay the same.
- Ant, Gradle, and Meson call `pkg-config --cflags` and `pkg-config --libs` separately. The host set is Windows, macOS, and Linux with glibc, uClibc, or musl. A missing `src/main/c` contributes no files.
- `project.h`, `project.ini`, `meson.build`, `build.gradle`, `build.xml`, and the README Version badge say 1.1.1.

## [1.1.0] - 2026-11-10

### Added
- A normal run keeps a dated log as plain text for 7 days. A day older than that becomes one package, `tar.gz` on this host and `zip` on Windows, and the plain file is removed only after the package is complete. A day older than 30 days is removed. The age is the date in the file name. The file the run is appending stays plain.
- `help`, `about`, `list`, `show`, and `test` do not archive a day and do not delete a day.
- `tests/test-cpp-log-retention.sh` proves the 7-day plain file, the package, the 30-day removal, and a failed package that leaves the plain file.
- `tests/test-cpp-logger-portable.sh` proves one name from `project.ini` and `project.h`.

### Changed
- `project.h`, `project.ini`, `meson.build`, `build.gradle`, `build.xml`, and the README Version badge say 1.1.0.
- The about page builds the cache path and the persistence path from `PROJECT_NAME`.

## [1.0.8] - 2026-11-09

### Added
- `about` prints the name, version `1.0.8`, and description, then stops.
- A normal run logs `Starting cpp-logger 1.0.8` and `Finished cpp-logger 1.0.8`.
- Debug mode logs each switch that was present, then `component main start` and `component main end`.
- Debug mode shows stdout log lines without `--verbose`. Quiet and JSON still hide the human line.
- Light actor catalog for the normal user who starts the program. There is no approver.
- GNU Make file `Makefile`. The public home is `https://github.com/cloudgen/cpp-logger`.
- Check `tests/test-run-boundary.sh` and checklist `docs/checklists/2026-11-09-checklist-run-boundary.md`.

### Changed
- `project.h`, `project.ini`, `meson.build`, and the README Version badge all say 1.0.8.
- Target operating systems recorded for the console program: Debian, Ubuntu, CentOS, Red Hat, Alpine, Buildroot, macOS, Windows, Fedora, and Rocky Linux. The checked build remains `g++` on Linux.

## [Unreleased]

### Added
- `about` prints three live facts: which program this is, what this computer is, and where it is running from. The blocks are identity, the host check, and the install location. `tests/test-cpp-about.sh` covers that page.

### Changed
- Law molds match the logger, output, and dated-file rules. `template-output-requirements.md` §3.2 holds the portable C++ bodies. The shell `out_*` rows are unchanged. Product requirements quote the matching functions from `src/main/cpp/logger.cpp` and `src/main/cpp/logger.h`.
- `docs/checklists/` keeps the two cpp-logger runs. The inherited genesis audit history is removed.
- The log checks name the refusal text. A missing directory switch, a missing datestamp, and a stamp that is not eight digits are separate failures. `show` of a missing file reports `cannot read log file`.
- The dated log file and the human line share `requirement-cpp-logger.md`. `requirement-cpp-log.md` is removed.
- Well-known log levels are `INFO`, `DEBUG`, `WARNING`, `ERROR`, and `FATAL`, the set VideoSpeed allows. `logger_fatal` emits `FATAL` on stderr. Quiet does not hide it, and `--verbose` is not required. `CRITICAL` is not a well-known level.
- `list` prints only regular `*.log` names. A missing log directory still prints nothing and exits 0. A log path that exists and cannot be opened exits 1. A stamp that is not eight digits reports `--log-datestamp requires YYYYMMDD`. A missing stamp on `test` or `show` stays a separate error. One invocation accepts one of `test`, `list`, or `show`.
- The app name is `cpp-logger`.
- Human log lines use the ChronicleLogger shape: `[YYYY-MM-DD HH:MM:SS] pid:<pid> [LEVEL] @main :] message`. `WARNING` is the warning level. `ERROR` goes to stderr.
- Ship-unit sources live in `src/main/cpp`: `logger.cpp`, `main.cpp`, `logger.h`, and `project.h`.
- Quiet mode is `logger_set_quiet` and `logger_get_quiet`. Debug is `logger_get_debug`. There is no debug setter. JSON mode keeps quiet on and still emits each allowed event as one JSON object. Quiet hides human info, warning, and debug lines.

### Added
- Project-base-path and project-log-path. The application directory is `/var/{{app-name}}` for root or sudo and `{{HOME}}/.app/{{app-name}}` for every other user. The log directory is that path plus `/log`. The C++ example is `cpp-logger`. The cache path stays a different directory: `{{HOME}}/.cache/cache-{{app-name}}/` or `/dev/shm/cache/cache-{{app-name}}` for a normal user, and `/var/cache/{{app-name}}/` for root or sudo.
- Log file `requirement-cpp-log.md`. `cpp-logger test --home /tmp --log-datestamp 20261003` appends `/tmp/.app/cpp-logger/log/cpp-logger-20261003.log`. `test --prefix /tmp` with the same stamp appends `/tmp/var/cpp-logger/log/cpp-logger-20261003.log`.
- cpp-base-path and cpp-log-path. Root or sudo uses `/var/cpp-logger` and `/var/cpp-logger/log`. Every other user uses `$HOME/.app/cpp-logger` and `$HOME/.app/cpp-logger/log`. Terms: `cpp-path`, `cpp-base-path`, `cpp-log-path`, `cpp-test-base-path`, `cpp-test-log-path`, `cpp-test-purpose-command`, `cpp-test-purpose-switch`.
- `--verbose` displays stdout log lines. Those lines are hidden by default. `--verbose` has no effect while quiet or JSON is on. The human error line stays on stderr. The dated file still receives the human line.
- Test purpose commands `list` and `show`. `list --home /tmp` prints the log file names. `show --home /tmp --log-datestamp 20261003` prints that dated file. Both require `--prefix` or `--home` and do not read `HOME` or `PREFIX`.
- Single-binary test `tests/test-logger-folder.sh` and checklist `docs/checklists/2026-10-03-checklist-logger-folder.md`. The check compiles one binary and runs the `test` verb. It does not copy the binary into an install bin. It does not use `mktemp`, `mkstemp`, or `HOME`.
- Global installation `requirement-cpp-global-installation.md`: `./build.sh install-global` copies the built binary to `$PREFIX/bin` (`/usr/local/bin` when `PREFIX` is unset) at mode `0755` and does not call `sudo`.
- Local installation `requirement-cpp-local-installation.md`: `./build.sh install-local` copies the built binary to `$HOME/.local/bin` at mode `0755`.
- Logger requirement `requirement-cpp-logger.md` owns the human ChronicleLogger line.
- JSON requirement `requirement-cpp-json.md`: JSON mode implies quiet, and the only product line is the JSON object.
- C++ build skills `SK-CPP-BUILD-ANT`, `SK-CPP-BUILD-GNU-MAKE`, `SK-CPP-BUILD-GRADLE`, and `SK-CPP-BUILD-MESON`. Ant, GNU Make, and Gradle stay off `./build.sh`.
- C++ coding-style requirement and law mold `template-cpp-coding-style.md`.
- C++ folder requirement `requirement-cpp-folder.md` and law mold `template-cpp-folder.md`. `src/main/cpp` is the only ship-unit source directory.
- Glossary terms `kebab-case` and `camel-case`.

### Removed
- GTK window, WebKit view, menu bar, URL bar, bookmarks, auto-follow, cookies, user-agent settings, `config.conf`, and Lua userscripts.
- UI requirements `requirement-cpp-window.md`, `requirement-cpp-ui-menu.md`, `requirement-cpp-webui-auto-follow.md`, and `requirement-cpp-webui-bookmark.md`.
- `requirement-cpp-milestones.md` and `requirement-cpp-lua-script.md`.
- `requirement-cpp-configuration.md`.
- `requirement-cpp-cookie-management.md` and `requirement-cpp-database.md`.
- sol2 sources and the GTK, WebKit, SQLite, and Lua link dependencies.

## [1.0.6] - 2026-04-18

### Added
- RECOMMENDATION.md

## [1.0.5] - 2026-04-18

### Added
- Persistent User-Agent settings using `settings.sqlite` (stored alongside cookies).
- User-Agent selected at startup now correctly displays in the UI label (previously showed default Chrome until changed).
- `webkit_setup.c/h` module — Single Point of Truth for all WebKitWebContext, cookie, and User-Agent initialization.
- `ui_window_gemini.c/h` and `ui_menu_gemini.c/h` modules for better separation of concerns.
- Strong CIAO defensive protection comments on critical functions (`create_main_window()`, `build_main_menu_bar()`, etc.).
- `PROJECT_DESCRIPTION` constant in `project.h` (Single Source of Truth).

### Changed
- `main.c` is now pure orchestration (very clean and short).
- `add_url_bar_user_agent()` now accepts an `initial_ua` parameter to respect persisted settings.
- Enhanced protection rules to preserve GitHub-specific UI modules when forking to other projects.
- Updated version dialog to use centralized `PROJECT_DESCRIPTION`.

### Fixed
- UI label under URL bar now shows correct persisted User-Agent on application startup.
- Duplicate loading of User-Agent settings at startup.

---

## [1.0.3] - 2026-04-18

### Added
- Full User-Agent spoofing menu (Chrome, Firefox, Safari, Edge) with Chrome as default.
- Persistent cookie storage using SQLite.
- Advanced output system with `--quiet`, `--json`, and `DEBUG=1` support.
- Generic bookmark system + GitHub-specific bookmarks.
- Real-time URL bar synchronization (`notify::uri` + `load-changed` fallback).
- Enter key support in URL bar.

### Changed
- Migrated to strong CIAO Defensive Programming style across the entire codebase.
- All User-Agent strings centralized in `project.h`.

---

## [1.0.0] - 2026-04-14

### Added
- Initial release of lightweight native GitHub wilgat using GTK3 + WebKit2GTK.
- Basic URL bar and navigation.
- Defensive coding foundation (CIAO principles).

---

## How to Maintain This File

When releasing a new version:
1. Move `[Unreleased]` content into a new version header with today's date.
2. Add a fresh `[Unreleased]` section at the top.
3. Use the standard categories: **Added**, **Changed**, **Fixed**, etc.

---

**Project**: [cloudgen/gemini-wilgat](https://gemini.com/cloudgen/gemini-wilgat)  
**Philosophy**: Built with strict CIAO Defensive Programming Principles

---

Made with ❤️ and extreme caution.