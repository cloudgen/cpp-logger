**file**: docs/requirements/requirement-cpp-folder.md
**Status**: Active
**Updated**: 2026-11-10
**Philosophy**: CIAO v2.10.2 / CIAO-Lite (Caution • Intentional • Anti-fragile • Over-engineered / Over-protect)

## 1. Purpose

This file owns the ship-unit directory `src/main/cpp`. Coding style owns spelling and linkage. Project structure owns the rest of the repository map. The builds compile this directory. They do not choose another source root.

### 1.1 Human-facing

**In one sentence:** You put the C++ sources in `src/main/cpp`, and that directory is the only ship-unit source root.

| Box | Meaning | Example |
|-----|---------|---------|
| You / this login | The person adding a source file | You add a `.cpp` file under `src/main/cpp` |
| The other role | `./build.sh`, Ant, Gradle, and Meson | They compile the translation units in that directory |
| Not this file | How a name is spelled, and what a log line looks like | `requirement-cpp-coding-style.md` and `requirement-cpp-logger.md` |

| Includes | Excludes |
|----------|----------|
| The directory path, the files that belong there, and the rule that `src/main/c` is not a source root | Flag lists, the ChronicleLogger line, and apt package names |

| Surface | What you open | What for |
|---------|---------------|----------|
| `src/main/cpp/` | Ship-unit sources | The files in §2.1 |
| `project.sh` | `SRC_C` | The path the simple build searches |

| You do… | What it means | What you type |
|---------|---------------|---------------|
| Find the sources | Open the only ship-unit directory | `src/main/cpp` |

## 2. Core rules

1. The only ship-unit source directory is `src/main/cpp`. There is no `src/main/c`.
2. A translation unit in that directory is `.cpp` unless this requirement names a `.c` file. This requirement names no `.c` file.
3. A header named in §2.1 stays `.h`. Do not rename `logger.h` or `project.h` to `.hpp`.
4. Shell scripts, Meson files, and Markdown do not go in this directory.
5. `project.sh` sets `SRC_C` to `src/main/cpp`. `./build.sh`, `build.xml`, `build.gradle`, and `meson.build` compile the `.cpp` files there. `meson.build` lists the same translation units. A new translation unit is added to this file and to the Meson source list in the same change. An Ant-generated `project.sh` is `requirement-cpp-ant.md`. A Meson-generated `project.sh` is `requirement-cpp-meson.md`. Those generators do not replace this checked-in file and do not create `src/main/c`.
6. A header and its translation unit stay paired. `project.h` holds identity macros only.
7. cpp-log-path is not a source directory. For root or sudo it is `/var/cpp-logger/log`. For every other user it is `$HOME/.app/cpp-logger/log`. The directory is `requirement-cpp-logger-folder.md`. The dated file in it is `requirement-cpp-logger.md`. Do not move `main.cpp` or `logger.cpp` into that directory.

### 2.1 Files in this directory

| File | Role |
|------|------|
| `main.cpp` | Program entry |
| `logger.cpp` | Logger implementation |
| `logger.h` | Logger declarations |
| `project.h` | Runtime identity |

### Sample (this project)

```text
src/main/cpp/
  main.cpp
  logger.cpp
  logger.h
  project.h
```

## 3. Protection rule

Future editors must not recreate `src/main/c`, add a second ship-unit source root, or move a named header off `.h`.

## 4. Related artifacts

| Artifact | Role |
|----------|------|
| `docs/requirements/index.md` | Registry |
| `docs/requirements/requirement-cpp-coding-style.md` | Spelling and linkage |
| `docs/requirements/requirement-cpp-project-structure.md` | Repository map outside this directory |
| `docs/requirements/requirement-cpp-logger-folder.md` | Log directory, not this source directory |
| `docs/requirements/requirement-cpp-logger.md` | Dated log file inside that directory |
| `build.sh` | Compiles this directory |
| `build.xml` | Ant compile of this directory |
| `build.gradle` | Gradle compile of this directory |
| `meson.build` | Meson source list |
| `project.sh` | `SRC_C=src/main/cpp` |
| `docs/requirements/requirement-cpp-ant.md` | Ant compile of `.c` and `.cpp`, and the generated exports |
| `docs/requirements/requirement-cpp-gradle.md` | Gradle compile of this directory |
| `docs/requirements/requirement-cpp-meson.md` | Meson source list and the generated exports |

**Last updated**: 2026-11-10
**Owner**: C++ source folder
