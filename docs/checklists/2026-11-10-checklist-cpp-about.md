# Checklist: about page

**Filled run** for `requirement-cpp-about.md` and `requirement-cpp-tests.md` §2.5.

**Date:** 2026-11-10

**Suite:** `tests/test-cpp-about.sh` exited 0. The script builds one binary under `/tmp/cpp-logger-about-work`. It does not call `mktemp`. It does not read, assign, or delete `HOME`. The child is started with `HOME` and `PREFIX` removed.

## Warning

Do not call `mktemp` or `mkstemp`. Do not read, expand, assign, or delete the home environment variable. Cleanup deletes `/tmp/.app/cpp-logger`, `/tmp/.app` when that directory is empty, and `/tmp/cpp-logger-about-work`. It does not delete `/tmp` and it does not delete a home directory.

## Checks

- [x] One `g++` binary at `/tmp/cpp-logger-about-work/cpp-logger`. The check runs as a normal user and does not run `./build.sh`
- [x] Identity is `cpp-logger 1.1.0`, the description, `Domain: No database. No configuration file.`, `Runtime tools: none`, and `Entry points: cpp-logger`
- [x] A blank line separates identity from the host check. The stamp ends `.000000`. Two spaces sit before `[CHECK SYSTEM]:`
- [x] The host-check labels stay in the requirement order. The compiler line starts with `g++`. The standard line is `C++17`. The operating system and architecture match `uname`
- [x] With `HOME` removed, the 1st fallback, the 2nd fallback, and persistence stay on the page with empty values. The preferred cache path is `/dev/shm/cache/cache-cpp-logger`
- [x] A blank line separates the host check from `Install location`. The global probe uses `/usr/local/bin/cpp-logger`. The local probe is `Local install: not found`. `Running from` is the running binary
- [x] The page has no Python, conda, pyenv, star-box install sentence, `properly installed`, Repository row, or Install channel row
- [x] `--quiet about` still prints the host check and `Install location`. `--json about`, `about --json`, and `JSON=1 about` each print one JSON object and do not print `[CHECK SYSTEM]` or `Install location`
- [x] The run creates no `/tmp/.app/cpp-logger` directory and no cache directory
- [x] `tests/test-cpp-about.sh` printed `PASS cpp about` on 2026-11-10
- [x] `tests/test-run-boundary.sh` printed `PASS run boundary` on 2026-11-10 after the install-location block

## Verdict

- [x] **Pass**
- [ ] **Revise**
- [ ] **Block**
