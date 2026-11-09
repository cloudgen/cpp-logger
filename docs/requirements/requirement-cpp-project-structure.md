**file**: docs/requirements/requirement-cpp-project-structure.md
**Status**: Active
**Updated**: 2026-10-03
**Philosophy**: CIAO v2.10.2 / CIAO-Lite (Caution • Intentional • Anti-fragile • Over-engineered / Over-protect)

## 1. Purpose

This file owns the repository map. The files inside `src/main/cpp` are owned by `requirement-cpp-folder.md`. Product identity comes from `project.h`.

### 1.1 Human-facing

**In one sentence:** The repository map places sources in `src/main/cpp/`, and the binary is written under `target/exe/debug/`.

| Box | Meaning | Example |
|-----|---------|---------|
| You / this login | The person who builds or runs the app | You open `src/main/cpp/` |
| The other role | The build directory | It holds the binary after `./build.sh build` |
| Not this file | Where the built binary is installed | `requirement-cpp-global-installation.md` and `requirement-cpp-local-installation.md` |

| Includes | Excludes |
|----------|----------|
| The repository map and the binary path | Which files belong in `src/main/cpp` (`requirement-cpp-folder.md`) |

| Surface | What you open | What for |
|---------|---------------|----------|
| Repository root | Source and `./build.sh` | Build |
| `target/exe/debug/` | Binary | Run |

| You do… | What it means | What you type |
|---------|---------------|---------------|
| Find the binary | Simple build writes it under `target/exe/debug/` | `./build.sh build` |

## 2. Core rules

Source repository:

```text
cpp-logger/
├── README.md
├── build.sh
├── project.sh
├── project.ini
├── meson.build
├── src/main/cpp/          # owned by requirement-cpp-folder.md
├── target/              # build output, not source
└── docs/requirements/   # this registry
```

A normal run creates cpp-log-path for the current environment: `/var/cpp-logger/log` for root or sudo, and `$HOME/.app/cpp-logger/log` for every other user. The test command creates cpp-test-log-path instead. Those directories are `requirement-cpp-logger-folder.md`. The dated file is `requirement-cpp-logger.md`. The program does not create `config.conf`, `settings.sqlite`, `cookies.sqlite`, or a userscripts directory.

Paths that a later feature creates use kebab-case tokens, as `requirement-cpp-coding-style.md` states. A literal product id other than `PROJECT_ID` in source is a defect.

### 2.1 Implementation notes (this project)

`PROJECT_ID` is `cpp-logger`. The simple-build binary is `target/exe/debug/cpp-logger`.

## 3. Protection rule

Future editors must not commit `target/` or put a cookie directory back into this layout.

## 4. Related artifacts

| Artifact | Role |
|----------|------|
| `docs/requirements/index.md` | Registry |
| `src/main/cpp/project.h` | `PROJECT_ID` |
| `docs/requirements/requirement-cpp-folder.md` | Files inside `src/main/cpp` |
| `docs/requirements/requirement-cpp-logger-folder.md` | cpp-base-path and cpp-log-path |
| `docs/requirements/requirement-cpp-logger.md` | The dated file in that directory |
| `docs/requirements/requirement-cpp-coding-style.md` | kebab-case for path tokens |

**Last updated**: 2026-10-03
**Owner**: Project structure
