**file**: docs/requirements/requirement-cpp-local-installation.md
**Status**: Active
**Updated**: 2026-10-03
**Philosophy**: CIAO v2.10.2 / CIAO-Lite (Caution • Intentional • Anti-fragile • Over-engineered / Over-protect)

## 1. Purpose

This file owns local installation: placing the built program in the current user's bin. The system-wide place stays in `requirement-cpp-global-installation.md`. Apt packages stay in `requirement-cpp-dependencies.md`.

### 1.1 Human-facing

**In one sentence:** You place `cpp-logger` in `$HOME/.local/bin` for this login, without root.

| Box | Meaning | Example |
|-----|---------|---------|
| You / this login | The person who wants the program on their own PATH | You run the local place after a build |
| The other role | `./build.sh install-local` | It creates the user bin if needed and copies the built binary |
| Not this file | The system bin and apt packages | `requirement-cpp-global-installation.md` and `requirement-cpp-dependencies.md` |

| Includes | Excludes |
|----------|----------|
| The user bin, the file mode, and creating that directory | `/usr/local/bin` and `./build.sh install` |

| Surface | What you open | What for |
|---------|---------------|----------|
| `~/.local/bin` | The installed program | Run it for this user |
| `target/exe/debug/cpp-logger` | The Linux debug binary | The file that is copied. The `download/` copy is not this install |

| You do… | What it means | What you type |
|---------|---------------|---------------|
| Install for this user | One binary at mode `0755` in the user bin | `./build.sh install-local` |

## 2. Core rules

1. The built file is the debug binary from `requirement-cpp-debug-binary.md`. This product's Linux file is `target/exe/debug/cpp-logger`. The command stops if that file is missing. It does not compile. It does not copy the `download/` payload file.
2. The destination directory is `$HOME/.local/bin`. That directory is the install bin. It is not cpp-log-path. cpp-log-path for every user other than root or sudo is `$HOME/.app/cpp-logger/log` and is owned by `requirement-cpp-logger-folder.md`. This operator command reads `HOME`. The installed path is `$HOME/.local/bin/cpp-logger`. If `HOME` is empty, the command stops. It does not read `USER_BIN`. Tests must not run this command and must not expand `HOME`. The test command passes `--home /tmp` and is `requirement-cpp-tests.md`.
3. The command creates the destination directory when it is missing. It does not create `/usr/local/bin`.
4. The installed file mode is `0755`. It is not `0711` and it is not `0700`.
5. This command does not call `sudo`, does not run `apt`, and does not edit shell startup files. Putting the user bin on `PATH` is the operator's shell setup, not this command.
6. `./build.sh install` remains the apt command. It is not local installation.

## 3. Protection rule

Future editors must not point local installation at `/usr/local/bin`, call `sudo` from `install-local`, or leave the installed file at mode `0711`.

## 4. Related artifacts

| Artifact | Role |
|----------|------|
| `docs/requirements/index.md` | Registry |
| `docs/requirements/requirement-cpp-logger-folder.md` | `$HOME/.app/cpp-logger/log`, which is not this bin |
| `docs/requirements/requirement-cpp-global-installation.md` | System-wide place |
| `docs/requirements/requirement-cpp-dependencies.md` | Apt packages via `./build.sh install` |
| `docs/requirements/requirement-cpp-project-structure.md` | Built binary path |
| `build.sh` | `install-local` |

**Last updated**: 2026-10-03
**Owner**: Local installation
