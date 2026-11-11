# Reviews

Product proof maps for cpp-logger.

| File | Role |
|------|------|
| `test-plan.md` | TP id, what a pass looks like, and status |
| `requirement-test-matrix.md` | Requirement to TP id |

Ant, Gradle, Meson, GNU Make, and the link set share `tests/test-cpp-builds.sh`. That script runs `ant compile`, `gradle assemble --offline`, and `make`. On 2026-11-11 the script exited 0 for release 1.2.1. `target/exe/debug/cpp-logger` and `download/cpp-logger` were the same bytes. TP-CPP-ANT, TP-CPP-ANT-STYLE, TP-CPP-GRADLE, TP-CPP-GRADLE-STYLE, TP-CPP-MESON, TP-CPP-MESON-STYLE, TP-CPP-DEPENDENCIES, and TP-CPP-DEBUG-BINARY are pass.

`cpp-logger about` is `tests/test-cpp-about.sh`. On 2026-11-11 that script exited 0 for `cpp-logger 1.2.1`. The page is identity, the host check, and the install location. TP-CPP-ABOUT is pass.

Log retention is `tests/test-cpp-log-retention.sh`. On 2026-11-11 that script exited 0. TP-CPP-LOG-RETENTION is pass. The portable name check is `tests/test-cpp-logger-portable.sh`. On 2026-11-11 that script exited 0 for `VERSION` `1.2.1`. TP-CPP-LOGGER-PORTABLE is pass. `tests/test-cpp-oop.sh` exited 0 for `class Invocation` and the loggable debug lines. TP-CPP-OOP is pass.
