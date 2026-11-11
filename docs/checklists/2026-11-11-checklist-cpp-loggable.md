# Checklist: Loggable components and release 1.2.0

**Filled run** for `requirement-cpp-oop.md`, `requirement-cpp-about.md`, `requirement-cpp-debug-mode.md`, `requirement-cpp-logger.md`, `requirement-cpp-logger-folder.md`, `requirement-cpp-json.md`, and `requirement-cpp-tests.md`.

**Date:** 2026-11-11

**Release:** 1.2.0

The 2026-11-11 quiet checklist remains the record for release 1.1.3. This file owns `Invocation`, the logger bind on `AboutCli` and `Help`, and the version triple `1.2.0`.

## Behavior

- [x] `invocation.h` declares `class Invocation` once. `Invocation::bind`, `Invocation::takeCommand`, and `Invocation::markComplete` are present
- [x] `AboutCli` and `Help` bind `Logger`. `aboutcli.cpp` does not name `logger_persistence_storage`
- [x] `help --debug` records `Invocation::bind`, `Invocation bound`, `Invocation page 1`, `Help::bind`, `Help bound`, `Help::printPage`, `Invocation::markComplete`, and `Invocation complete`, and does not print `Created directory`
- [x] `help --json --debug` is one object whose command is `help` and whose text does not contain `Help::`
- [x] `about --debug` records `AboutCli::bind`, `AboutCli bound`, `AboutCli::printPage`, and `Invocation page 2`, and the page starts with `cpp-logger 1.2.0`
- [x] `list --debug` records `Invocation purpose 2` and does not create the log directory
- [x] Plain `help` does not contain `Help::bind`
- [x] GNU Make writes `target/exe/debug/cpp-logger`, and `about` from that binary prints `cpp-logger 1.2.0`

## Suites

- [x] `tests/test-cpp-oop.sh` printed `PASS cpp oop` and exited 0
- [x] `tests/test-cpp-about.sh` printed `PASS cpp about` and exited 0 for `cpp-logger 1.2.0`
- [x] `tests/test-run-boundary.sh` printed `PASS run boundary` and exited 0 for `Starting cpp-logger 1.2.0` and `Finished cpp-logger 1.2.0`
- [x] `tests/test-logger-folder.sh` printed `PASS logger folder` and exited 0
- [x] `tests/test-log-verbs.sh` printed `PASS log verbs` and exited 0
- [x] `tests/test-cpp-log-retention.sh` printed `PASS cpp log retention` and exited 0
- [x] `tests/test-cpp-logger-portable.sh` printed `PASS cpp logger portable` and exited 0
- [x] `tests/test-cpp-builds.sh` printed `PASS cpp builds` and exited 0

## Verdict

- [x] **Pass** — the loggable debug lines, the reader directory rule, and `cpp-logger 1.2.0` exited 0 on 2026-11-11
- [ ] **Revise**
- [ ] **Block**
