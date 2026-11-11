**file**: docs/requirements/requirement-cpp-debug-binary.md
**Status**: Active
**Updated**: 2026-11-11
**Philosophy**: CIAO v2.10.2 / CIAO-Lite (Caution • Intentional • Anti-fragile • Over-engineered / Over-protect)

## 1. Purpose

This file owns the debug binary path and the payload-download copy. `./build.sh`, GNU Make, Ant, Gradle, and Meson write that same file. Host flags, pkg-config, and verb names stay in their own requirements. `./build.sh` stays the primary build and does not gain a `make`, `ant`, `gradle`, or `meson` verb.

### 1.1 Human-facing

**In one sentence:** A successful link writes `target/exe/debug/<name>` on Linux and macOS, `target/exe/debug/<name>.exe` on Windows, and copies those same bytes to `download/` under the same file name.

| Box | Meaning | Example |
|-----|---------|---------|
| You / this login | The person who builds | `./build.sh build`, then the file under `download/` |
| The other role | The five compile hosts | They write one debug binary and one download copy |
| Not this file | Compiler flags, pkg-config, and verb banners | `requirement-cpp-ant.md`, `requirement-cpp-gradle.md`, `requirement-cpp-meson.md`, `requirement-cpp-build-sh.md` |

| Includes | Excludes |
|----------|----------|
| The debug directory, the host file name, the `.exe` suffix on Windows, and the `download/` copy | GTK, WebKit, a second compile, and a tool verb on `./build.sh` |

| Surface | What you open | What for |
|---------|---------------|----------|
| `project.ini` | `PRJ` | The file name |
| `target/exe/debug/` | The linked binary | Run |
| `download/` | The same bytes | Payload download |

| You do… | What it means | What you type |
|---------|---------------|---------------|
| Build | One link, then one copy | `./build.sh build` or `make` |
| Fetch the payload | Read the copy under `download/` | The file named below |

## 2. Core rules

1. The file name is the `PRJ` key in `project.ini`. When the key is missing, the name is `cpp-logger`. Linux and macOS use that name with no suffix. Windows appends `.exe`. The directory of the linked binary is `target/exe/debug`.
2. The Windows suffix is chosen by the host that is actually running. `./build.sh` appends `.exe` when `uname -s` matches `MINGW`, `MSYS`, `CYGWIN`, or `Windows_NT`, or when `OS` is `Windows_NT`. GNU Make appends `.exe` when `OS` is `Windows_NT`. Ant appends `.exe` when the `os` family is `windows`. Gradle appends `.exe` when `os.name` contains `windows`. Meson `executable()` appends `.exe` on Windows by itself, and the download name uses the same suffix when `host_machine.system()` is `windows`.
3. After a successful link, the same bytes are copied to `download/` with the same file name. That copy is the payload download. It is not a second compile. A later successful link overwrites it. A host task that skips the link names the debug file and the download file and does not write either file.
4. The five hosts are `./build.sh`, the root `Makefile`, `build.xml`, `build.gradle`, and `meson.build`. Each one writes the file in rule 1. `./build.sh` remains primary. It does not gain a `make`, `ant`, `gradle`, or `meson` verb.
5. `download/` is generated output. `.gitignore` ignores `/download/`. This requirement names the path a payload download fetches after a build. The compiled bytes are not committed.
6. `./build.sh clean`, `make clean`, `ant clean`, and `gradle clean` remove the download copy. `meson compile --clean` removes the setup directory. The next Meson compile overwrites the download copy. Install commands copy the debug binary from rule 1. They do not copy the `download/` file and they do not compile.

### 2.1 Implementation notes (this project)

`project.ini` sets `PRJ=cpp-logger`.

| Host | Debug binary | Payload download |
|------|----------------|------------------|
| Linux | `target/exe/debug/cpp-logger` | `download/cpp-logger` |
| macOS | `target/exe/debug/cpp-logger` | `download/cpp-logger` |
| Windows | `target/exe/debug/cpp-logger.exe` | `download/cpp-logger.exe` |

On this Linux tree the proof compares `target/exe/debug/cpp-logger` with `download/cpp-logger`. Windows and macOS keep the names in the table. A Linux run does not produce the `.exe` file.

### 2.2 Why this requirement exists

- **CIAO Principle 5** (https://github.com/cloudgen/ciao): one path, read from `project.ini`, shared by every compile host.
- **CIAO Principle 1**: a missing link does not invent a download file.

## 3. Design principles

- **Caution**: Windows and macOS host tasks that skip pkg-config do not pretend the link ran.
- **Intentional**: the download copy is the same bytes as the debug binary.
- **Anti-fragile**: the name follows `PRJ`, with `.exe` only on Windows.

## 4. Verification

| TP-ID | Pass |
|-------|------|
| TP-CPP-DEBUG-BINARY | `build.sh`, `Makefile`, `build.xml`, `build.gradle`, and `meson.build` each name `target/exe/debug`, `download`, and `.exe`. On Linux, `ant compile`, `gradle assemble`, and `make` each leave `target/exe/debug/${PRJ}` and `download/${PRJ}` as the same bytes. The script does not run `meson`, does not call `mktemp`, and does not read, assign, or delete `HOME` |

### Design-time verification

| TP-ID | Suite | Status |
|-------|-------|--------|
| TP-CPP-DEBUG-BINARY | `tests/test-cpp-builds.sh` | pass |

**Matrix:** `docs/reviews/requirement-test-matrix.md`
**Map:** `docs/reviews/test-plan.md`

## 5. Protection rule

Future editors must not invent a second debug directory, must not drop the Windows `.exe` suffix, must not drop the `download/` copy, must not commit that copy, must not give `./build.sh` a `make`, `ant`, `gradle`, or `meson` verb, and must not point this path at GTK or WebKit. The product name stays in `project.ini` and `src/main/cpp/project.h`.
