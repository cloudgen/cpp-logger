# Test plan

Proof map for cpp-logger. A row stays todo until its check has been run.

The GTK window, WebKit view, cookies, bookmarks, user-agent menu, userscripts, and `config.conf` checks are removed with those features.

| TP-ID | Requirement | What a pass looks like | Status |
|-------|-------------|------------------------|--------|
| TP-CPP-DEPENDENCIES | requirement-cpp-dependencies | `pkg-config --exists glib-2.0` succeeds. `build.sh` and `meson.build` both request that module and do not request `gtk+-3.0` or `webkit2gtk-4.1` | todo |
| TP-CPP-LOGGER-FOLDER | requirement-cpp-logger-folder, requirement-cpp-logger, requirement-cpp-output | `tests/test-logger-folder.sh` builds one binary. A missing directory switch, a missing stamp, and a short stamp each exit 1 with their own sentence and create no file. `--debug` without `--verbose` shows `switch --debug`, `component main start`, `component main end`, and `test log` on stdout and in the file, and does not show `Starting cpp-logger`. `--verbose` shows the log directory. `--verbose` shows nothing human with `--quiet` or `--json`. `--home /tmp` writes `/tmp/.app/cpp-logger/log/cpp-logger-20261003.log`. `--prefix /tmp` writes `/tmp/var/cpp-logger/log/cpp-logger-20261003.log`. Passed 2026-11-09 | pass |
| TP-CPP-LOG-VERBS | requirement-cpp-logger, requirement-cpp-tests | `tests/test-log-verbs.sh` builds one binary. `list --home /tmp` prints `cpp-logger-20261002.log` then `cpp-logger-20261003.log`, and skips `notes.txt` and a directory named `extra.log`. `show` prints that file and does not change it. `--quiet` and `--json` still print it. A missing directory switch, a short stamp, and a second purpose verb exit 1. `--prefix` wins. `PREFIX` is ignored. A missing file exits 1 with `cannot read log file`. A log path that is a file exits 1. Passed 2026-10-03 | pass |
| TP-CPP-RUN-BOUNDARY | requirement-cpp-logger, requirement-cpp-debug-mode, requirement-cpp-parameters, requirement-cpp-tests | `tests/test-run-boundary.sh` builds one binary. `about` prints `cpp-logger 1.0.8` and `No database. No configuration file.` `help` prints `about`. A normal run with no flags prints no human line. `--verbose` prints the starting and finished lines. `--debug` prints the switch line, the component pair, and those two lines. Quiet and JSON hide the human line. The dated test file stores the switch lines and `test log` and does not store `Starting cpp-logger`. Passed 2026-11-09 | pass |

**Last updated**: 2026-11-09
