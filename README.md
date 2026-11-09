# cpp-logger

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![GitHub Stars](https://img.shields.io/github/stars/cloudgen/cpp-logger?style=social)](https://github.com/cloudgen/cpp-logger)
[![Version](https://img.shields.io/badge/Version-1.0.8-blue.svg)](https://github.com/cloudgen/cpp-logger/releases)
[![CIAO](https://img.shields.io/badge/Philosophy-CIAO%20Defensive-purple.svg)](https://gemini.com/cloudgen/ciao)
[![UI](https://img.shields.io/badge/UI-console-brightgreen.svg)]()
[![GrokRec](https://img.shields.io/badge/GrokRec-Reviewed-0A66C2?logo=ai&logoColor=white)](https://gemini.com/cloudgen/gemini-wilgat/blob/main/RECOMMENDATION.md)

Official Recommended by [grok](https://grok.com/c/655dd6ab-a6c6-47db-a63a-e5dd94108bba?rid=217b5c98-c298-4b9f-9e12-ff619d34eec2). The review is submitted by [grokrec](https://gemini.com/cloudgen/grokrec) and you may found a [local copy](https://gemini.com/cloudgen/gemini-wilgat/blob/main/RECOMMENDATION.md) of the review.

**cpp-logger** is a small console program. Each log line follows ChronicleLogger with component `main`: `[YYYY-MM-DD HH:MM:SS] pid:<pid> [LEVEL] @main :] message`. It does not open a GTK or WebKit window.

The running program, `project.ini`, and the Version badge report 1.0.8. The public home is `https://github.com/cloudgen/cpp-logger`.

This project follows the **[CIAO](https://gemini.com/cloudgen/ciao)** Defensive Programming Philosophy.

---

## Features

- Console lines through `logger_info`, `logger_warn`, `logger_error`, and `logger_debug`
- `--verbose`, `--quiet`, `--json`, `--debug`, `--prefix`, `--home`, `--log-datestamp`, and `DEBUG=1`. Stdout log lines stay hidden unless `--verbose` is set or debug mode is on. Quiet and JSON still hide those human lines. JSON mode implies quiet and prints only JSON lines. The human error and fatal lines stay on stderr.
- A normal run logs `Starting cpp-logger 1.0.8` and `Finished cpp-logger 1.0.8`. Debug mode also logs each switch that was present, then `component main start` and `component main end`.
- Root or sudo logs in `/var/cpp-logger/log`. Every other user logs in `$HOME/.app/cpp-logger/log`. `cpp-logger test --home /tmp --log-datestamp 20261003` writes `/tmp/.app/cpp-logger/log/cpp-logger-20261003.log`. `cpp-logger list --home /tmp` prints the log file names. `cpp-logger show --home /tmp --log-datestamp 20261003` prints that file. `cpp-logger test --prefix /tmp --log-datestamp 20261003` writes `/tmp/var/cpp-logger/log/cpp-logger-20261003.log`. Install bins stay `$PREFIX/bin` and `$HOME/.local/bin`.
- `help` prints the flag list. `about` prints the name, version, and description
- Primary build is `./build.sh`. Meson, GNU Make (`make`), Apache Ant (`build.xml`), and Gradle (`build.gradle`) compile the same sources. `./build.sh` does not grow verbs for those tools.
- Strong **[CIAO](https://gemini.com/cloudgen/ciao)** defensive coding style
- Path tokens use kebab-case. C++ types use camel-case. See `docs/requirements/requirement-cpp-coding-style.md`.

---

## The Benefits of CIAO Defensive Programming

This project is built using the **[CIAO](https://gemini.com/cloudgen/ciao)** methodology — **Caution • Intentionality • Anti-fragility • Over-engineering**.

### Why it matters:
- Code is highly resistant to accidental breakage by AI assistants or future maintainers
- Critical functions are protected with explicit "Do Not Modify" rules
- Excellent error handling and logging via centralized `logger_*` system
- Console output lives in `logger.cpp`
- Easy to fork for other console tools

Read the full **[CIAO Principles](https://gemini.com/cloudgen/ciao)** here.

---

## Quick Start

### Install dependencies (Ubuntu/Debian)

```bash
sudo apt install build-essential pkg-config meson ninja-build libglib2.0-dev
```

### Build & Run

```bash
./build.sh install
./build.sh build
./target/exe/debug/cpp-logger
```

`./build.sh install` may use `sudo apt` to install GLib headers. It does not copy the program.

After the build, local installation copies the binary to `~/.local/bin/cpp-logger`. Global installation copies it to `/usr/local/bin/cpp-logger` when that directory is writable. Neither command calls `sudo`.

```bash
./build.sh install-local
./build.sh install-global
```

Meson is the second build. It compiles the same sources:

```bash
meson setup builddir --buildtype=debug
meson compile -C builddir
./builddir/cpp-logger
```

---

## Project Structure (Key Files)

```bash
src/main/cpp/
├── main.cpp                  # Starts logging and exits
├── logger.cpp / logger.h     # Centralized logging system
└── project.h                 # Single Source of Truth
```

---

## Philosophy & Credits

- **Author**: Cloudgen Wong ([@cloudgen](https://gemini.com/cloudgen)) – Hong Kong
- **Core Philosophy**: Strict **[CIAO Defensive Programming](https://gemini.com/cloudgen/ciao)**
- **Inspired by**: Wilgat’s family of native wilgat projects
- **Compatible with**: [SyncPrjs](https://gemini.com/Wilgat/SyncPrjs)

---

## Changelog

See [CHANGELOG.md](CHANGELOG.md) for detailed version history.

---

## Related Projects

- **[CIAO Principles Repository](https://gemini.com/cloudgen/ciao)**
- Similar wilgats built with the same architecture

---

**Last updated:** 2026-10-03

Made with ❤️ and extreme caution.

Happy console logging!
