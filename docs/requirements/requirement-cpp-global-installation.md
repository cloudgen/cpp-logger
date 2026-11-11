**file**: docs/requirements/requirement-cpp-global-installation.md
**Status**: Active
**Updated**: 2026-10-03
**Philosophy**: CIAO v2.10.2 / CIAO-Lite (Caution • Intentional • Anti-fragile • Over-engineered / Over-protect)

## 1. Purpose

This file owns global installation: placing the built program where every user on the host can run it. Apt packages stay in `requirement-cpp-dependencies.md`. The per-user place stays in `requirement-cpp-local-installation.md`.

### 1.1 Human-facing

**In one sentence:** You place `cpp-logger` in `$PREFIX/bin`, which is `/usr/local/bin` when `PREFIX` is unset.

| Box | Meaning | Example |
|-----|---------|---------|
| You / this login | The elevated operator who can write the system bin | You run the global place after a build |
| The other role | `./build.sh install-global` | It copies the built binary and sets its mode |
| Not this file | Apt packages and the per-user bin | `requirement-cpp-dependencies.md` and `requirement-cpp-local-installation.md` |

| Includes | Excludes |
|----------|----------|
| The global install bin, the file mode, and the rule that this command does not call `sudo` | `./build.sh install` (apt), the local bin, and cpp-log-path |

| Surface | What you open | What for |
|---------|---------------|----------|
| `$PREFIX/bin` | The installed program | Run `cpp-logger` for every user of that prefix |
| `target/exe/debug/cpp-logger` | The Linux debug binary | The file that is copied. The `download/` copy is not this install |

| You do… | What it means | What you type |
|---------|---------------|---------------|
| Install for every user of the prefix | One binary at mode `0755` in `<prefix>/bin` | `./build.sh install-global` |

## 2. Core rules

1. The built file is the debug binary from `requirement-cpp-debug-binary.md`. This product's Linux file is `target/exe/debug/cpp-logger`. The command stops if that file is missing. It does not compile. It does not copy the `download/` payload file.
2. The destination directory is `$PREFIX/bin`. That directory is the install bin. It is not cpp-log-path. cpp-log-path for root or sudo is `/var/cpp-logger/log` and is owned by `requirement-cpp-logger-folder.md`. This command reads `PREFIX`. When `PREFIX` is unset, the prefix is `/usr/local`, so the file is `/usr/local/bin/cpp-logger`. Termux sets `PREFIX` to its own prefix. The installed path is `$PREFIX/bin/cpp-logger`. This command does not create `/var/cpp-logger/log`.
3. The installed file mode is `0755`. It is not `0711` and it is not `0700`.
4. `<prefix>/bin` must already be writable by the user who runs the command. If it is missing or not writable, the command stops. It does not call `sudo`, and it does not create that directory.
5. This command does not copy into `~/.local/bin`, does not run `apt`, and does not edit shell startup files.
6. `./build.sh install` remains the apt command. It is not global installation.

## 3. Protection rule

Future editors must not point global installation at a per-user bin, call `sudo` from `install-global`, or leave the installed file at mode `0711`.

## 4. Related artifacts

| Artifact | Role |
|----------|------|
| `docs/requirements/index.md` | Registry |
| `docs/requirements/requirement-cpp-logger-folder.md` | `/var/cpp-logger/log`, which is not this bin |
| `docs/requirements/requirement-cpp-local-installation.md` | Per-user place |
| `docs/requirements/requirement-cpp-dependencies.md` | Apt packages via `./build.sh install` |
| `docs/requirements/requirement-cpp-project-structure.md` | Built binary path |
| `build.sh` | `install-global` |

**Last updated**: 2026-10-03
**Owner**: Global installation
