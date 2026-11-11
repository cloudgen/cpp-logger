**file**: docs/requirements/requirement-cpp-project-structure.md
**Status**: Active
**Updated**: 2026-11-11
**Philosophy**: CIAO v2.10.2 / CIAO-Lite (Caution • Intentional • Anti-fragile • Over-engineered / Over-protect)

## 1. Purpose

This file owns the repository map. The files inside `src/main/cpp` are owned by `requirement-cpp-folder.md`. Product identity macros come from `project.h`. `About` loads those macros and is the runtime source of the name, the version, the paths, and the flags.

### 1.1 Human-facing

**In one sentence:** The repository map places sources in `src/main/cpp/`, the checks in `tests/`, and the binary under `target/exe/debug/`.

| Box | Meaning | Example |
|-----|---------|---------|
| You / this login | The person who builds or runs the app | You open `src/main/cpp/` |
| The other role | The build directory | It holds the binary after `./build.sh build` |
| Not this file | Which files belong in `src/main/cpp`, and where the binary is installed | `requirement-cpp-folder.md`, `requirement-cpp-global-installation.md`, and `requirement-cpp-local-installation.md` |

| Includes | Excludes |
|----------|----------|
| The repository map, the four build files, `tests/`, and the binary path | Which files belong in `src/main/cpp` (`requirement-cpp-folder.md`) |

| Surface | What you open | What for |
|---------|---------------|----------|
| Repository root | `build.sh`, `build.xml`, `build.gradle`, and `meson.build` | Build |
| `tests/` | The shell checks | `requirement-cpp-tests.md` |
| `target/exe/debug/` | Binary | Run |

| You do… | What it means | What you type |
|---------|---------------|---------------|
| Find the binary | The simple build writes it under `target/exe/debug/` | `./build.sh build` |
| Find a check | The shell checks live in `tests/` | `tests/test-logger-folder.sh` |

## 2. Core rules

Source repository:

```text
cpp-logger/
├── README.md
├── CHANGELOG.md
├── LICENSE.md
├── SECURITY.md
├── CIAO-PRINCIPLES.md
├── RECOMMENDATION.md
├── config-sample.conf
├── .gitignore
├── build.sh
├── build.xml
├── build.gradle
├── meson.build
├── project.sh
├── project.ini
├── src/main/cpp/          # owned by requirement-cpp-folder.md
├── tests/                 # owned by requirement-cpp-tests.md
│   ├── test-logger-folder.sh
│   ├── test-log-verbs.sh
│   ├── test-run-boundary.sh
│   └── test-cpp-about.sh
├── docs/                  # document tree
│   └── requirements/      # this registry
├── target/                # build output, not source
├── build/                 # build output, not source
└── .gradle/               # build output, not source
```

`src/main/cpp` is the only source directory. There is no `src/main/c`. `About` in that directory owns the name, the version, the paths, and the flags. It parses, binds `Logger`, routes one verb, and finishes. The four build files compile that directory. `build.sh` is the primary build. `build.xml`, `build.gradle`, and `meson.build` are the Ant, Gradle, and Meson builds. Their host and flag rules are `requirement-cpp-ant.md`, `requirement-cpp-gradle.md`, and `requirement-cpp-meson.md`.

`tests/` holds `test-logger-folder.sh`, `test-log-verbs.sh`, `test-run-boundary.sh`, and `test-cpp-about.sh`. What those checks prove is `requirement-cpp-tests.md`.

`config-sample.conf` is a sample in the source tree. A normal run creates cpp-log-path for the current environment: `/var/cpp-logger/log` for root or sudo, and `$HOME/.app/cpp-logger/log` for every other user. The test command creates cpp-test-log-path instead. Those directories are `requirement-cpp-logger-folder.md`. The dated file is `requirement-cpp-logger.md`. The program does not create `config.conf`, `settings.sqlite`, `cookies.sqlite`, or a userscripts directory.

`target/`, `build/`, and `.gradle/` are build output. `.gitignore` excludes them. They are not source.

Paths that a later feature creates use kebab-case tokens, as `requirement-cpp-coding-style.md` states. A literal product id other than `PROJECT_ID` in source is a defect.

### 2.1 Implementation notes (this project)

`PROJECT_ID` is `cpp-logger`. The simple-build binary is `target/exe/debug/cpp-logger`. Ant, Gradle, and Meson use that same debug path. `docs/requirements/` is the product-law registry. The other directories under `docs/` are the harness around that registry.

## 3. Protection rule

Future editors must not commit `target/`, `build/`, or `.gradle/`, must not add `src/main/c` as a second source root, and must not put a cookie directory back into this layout.

## 4. Related artifacts

| Artifact | Role |
|----------|------|
| `docs/requirements/index.md` | Registry |
| `src/main/cpp/project.h` | `PROJECT_ID` |
| `docs/requirements/requirement-cpp-folder.md` | Files inside `src/main/cpp` |
| `docs/requirements/requirement-cpp-logger-folder.md` | cpp-base-path and cpp-log-path |
| `docs/requirements/requirement-cpp-logger.md` | The dated file in that directory |
| `docs/requirements/requirement-cpp-coding-style.md` | kebab-case for path tokens |
| `docs/requirements/requirement-cpp-tests.md` | The three files in `tests/` |
| `build.sh` | Primary build |
| `build.xml` | Ant build |
| `build.gradle` | Gradle build |
| `meson.build` | Meson build |
| `.gitignore` | Excludes `target/`, `build/`, and `.gradle/` |

**Last updated**: 2026-11-11
**Owner**: Project structure
