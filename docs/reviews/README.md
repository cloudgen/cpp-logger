# Reviews

Product proof maps for cpp-logger.

| File | Role |
|------|------|
| `test-plan.md` | TP id, what a pass looks like, and status |
| `requirement-test-matrix.md` | Requirement to TP id |

Ant, Gradle, and Meson share `tests/test-cpp-builds.sh`. That script runs `ant compile` and `gradle assemble --offline`. On 2026-11-10 those compiles exited 0. The script still exited 1 on the law-token checks. The six TP rows stay todo until the script exits 0 for each id.

`cpp-logger about` is `tests/test-cpp-about.sh`. On 2026-11-10 that script exited 0. The page is identity, the host check, and the install location. TP-CPP-ABOUT is pass.

Log retention is `tests/test-cpp-log-retention.sh`. On 2026-11-10 that script exited 0. TP-CPP-LOG-RETENTION is pass. The portable name check is `tests/test-cpp-logger-portable.sh`. On 2026-11-10 that script exited 0. TP-CPP-LOGGER-PORTABLE is pass.
