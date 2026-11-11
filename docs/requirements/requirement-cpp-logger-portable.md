**file**: docs/requirements/requirement-cpp-logger-portable.md
**Status**: Active
**Updated**: 2026-11-11
**Philosophy**: CIAO v2.10.2 / CIAO-Lite (Caution • Intentional • Anti-fragile • Over-engineered / Over-protect)

## 1. Purpose

This file owns the logger as a notebook a later project can adopt.

The intention is one shape and one name. ChronicleLogger takes the name as `logname`. Construction is `requirement-cpp-logger.md` §2.0: an explicit directory, then the conda prefix, the pyenv virtualenv, or `VIRTUAL_ENV`, each plus `/.app/<name>`, else `/var/<name>` for effective user id 0 and `$HOME/.app/<name>` otherwise. A later C++ project keeps that habit: it supplies its own name, version, and component. The line, the six levels, the verbs, the quiet and JSON gates, and the 7-day and 30-day life of a day stay the same notebook. The name is the only part that changes.

This product is one adopter. Its name is `cpp-logger`. The dated file, the directories, and the start and finish lines in `requirement-cpp-logger.md` and `requirement-cpp-logger-folder.md` are that name filled into this shape. A later project writes its own name into the same blanks. It does not copy the string `cpp-logger` into its directories or its file names.

### 1.1 Human-facing

**In one sentence:** A later project keeps this notebook and writes its own name on the cover.

| Box | Meaning | Example |
|-----|---------|---------|
| You / a later project | The product that adopts the notebook | Its name is the one on its project record |
| The other role | The logger | It stamps that name on the directory, the file, and the start line |
| Not this file | This product's filled-in paths | `requirement-cpp-logger.md` and `requirement-cpp-logger-folder.md` |

| Includes | Excludes |
|----------|----------|
| The blanks a later project fills: name, version, component | A second logger, a second name, and a change to the line shape |

| Surface | What you open | What for |
|---------|---------------|----------|
| The project record | The one name | Fill every blank with that name |
| The log directory | `/var/<name>/log` or `$HOME/.app/<name>/log` when no prefix is active | See that the cover matches the project |

| You do… | What it means | What you see |
|---------|---------------|--------------|
| Adopt the notebook | The later project brings its name, version, and component | `<name>-<YYYYMMDD>.log` |
| Stay on this product | The name is `cpp-logger` | `cpp-logger-20261110.log` |

## 2. Core rules

1. The logger has one name. That name is the adopting project's runtime name. This product reads it from `PRJ` in `project.ini` and from `PROJECT_NAME` in `src/main/cpp/project.h`. Those two strings are the same string. A later project keeps that pair and changes both together.
2. The version is the adopting project's version triple. This product reads `VERSION_MAJOR`, `VERSION_MINOR`, and `VERSION_PATCH` in `project.h`, and that triple matches `VERSION` in `project.ini`.
3. The component is the adopting project's `LOG_COMPONENT`. This product passes `main`. The writer follows the optional-component line in `requirement-cpp-logger.md` and does not insert that name when the caller passes an empty component.
4. The human line stays the line in `requirement-cpp-logger.md`. The levels stay `INFO`, `WARNING`, `ERROR`, `CRITICAL`, `FATAL`, and `DEBUG`. The name appears in the message when the message names the program. It does not appear as a second bracket.
5. An explicit base directory wins. Otherwise the first match wins: the active conda prefix plus `/.app/<name>`, else the active pyenv virtualenv plus `/.app/<name>`, else `VIRTUAL_ENV` plus `/.app/<name>`. When none of those is set, effective user id 0 uses `/var/<name>` and the log directory `/var/<name>/log`. Every other user uses `$HOME/.app/<name>` and `$HOME/.app/<name>/log`. When the base is chosen and the log directory was not explicit, the log directory is that base plus `/log`. The cache directory stays a different path, with the same name: `$HOME/.cache/cache-<name>`, `/dev/shm/cache/cache-<name>`, or `/var/cache/<name>` for root or sudo.
6. The dated file is `<name>-<YYYYMMDD>.log` inside that log directory. A test stand-in uses `--prefix <dir>` for `<dir>/var/<name>/log` and `--home <dir>` for `<dir>/.app/<name>/log`.
7. A normal run appends `Starting <name> <major>.<minor>.<patch>` and `Finished <name> <major>.<minor>.<patch>`.
8. Retention uses the same name. The plain file is `<name>-<YYYYMMDD>.log`. The package is `<name>-<YYYYMMDD>.log.tar.gz`, or `<name>-<YYYYMMDD>.log.zip` on Windows. The windows stay 7 days and 30 days, owned by `requirement-cpp-log-retention.md`.
9. The verbs stay `help`, `about`, `test`, `list`, and `show`. Help text prints `<name>` where this product prints `cpp-logger`.
10. A later project adopts this file and writes its own product requirements with its name filled in. It leaves this tree's requirements as the worked example for `cpp-logger`.

### This product's binding

| Blank | This product |
|-------|----------------|
| `<name>` | `cpp-logger` |
| Version | `1.2.1` |
| Component | `main` |
| Dated file | `cpp-logger-<YYYYMMDD>.log` |
| Log directory, other user | `$HOME/.app/cpp-logger/log` when no prefix is active |
| Log directory, root or sudo | `/var/cpp-logger/log` when no prefix is active |

## 3. Verification

| TP-ID | Pass |
|-------|------|
| TP-CPP-LOGGER-PORTABLE | `tests/test-cpp-logger-portable.sh` exited 0 on 2026-11-10. The check reads one name from `project.ini` and from `project.h` and expects those strings to match. The dated file, the log directory, and the help line use that name. A second literal product name in those paths is a failure. This tree's name is `cpp-logger` |

### Design-time verification

| TP-ID | Suite | Status |
|-------|-------|--------|
| TP-CPP-LOGGER-PORTABLE | `tests/test-cpp-logger-portable.sh` | pass |

**Matrix:** `docs/reviews/requirement-test-matrix.md`
**Map:** `docs/reviews/test-plan.md`

`tests/test-cpp-logger-portable.sh` proves this product's one name. It does not build a second project.

## 4. Protection rule

Future editors must not:

1. Give the logger a second name beside `PRJ` and `PROJECT_NAME`.
2. Hard-code a later project's name into this tree.
3. Change the line shape, the level set, or the 7-day and 30-day windows in order to make the notebook portable.
4. Mark `TP-CPP-LOGGER-PORTABLE` pass before a suite has exited 0.
5. Prove a later project by writing into that project's tree from this session.

**Violating this rule is a critical structure regression.**

## 5. Related artifacts

| Artifact | Role |
|----------|------|
| `docs/requirements/index.md` | Registry |
| `docs/requirements/requirement-cpp-logger.md` | This product's line and dated file |
| `docs/requirements/requirement-cpp-logger-folder.md` | This product's directories |
| `docs/requirements/requirement-cpp-log-retention.md` | The 7-day package and the 30-day removal |
| `project.ini` | `PRJ` and `VERSION` for this product |
| `src/main/cpp/project.h` | `PROJECT_NAME`, the version triple, and `LOG_COMPONENT` |
| ChronicleLogger `logname` | The sibling habit of one name on the notebook |

**Last updated**: 2026-11-11
**Owner**: Logger
