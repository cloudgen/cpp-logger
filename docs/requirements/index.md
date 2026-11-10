# Requirements registry

Product law for cpp-logger. Rows match files in this folder.

| Key | Area | Status | Path |
|-----|------|--------|------|
| requirement-actor-role-subject | architecture | Active | requirement-actor-role-subject.md |
| requirement-class-software-dev | class | Active | requirement-class-software-dev.md |
| requirement-cpp-coding-style | coding | Active | requirement-cpp-coding-style.md |
| requirement-cpp-oop | oop | Active | requirement-cpp-oop.md |
| requirement-cpp-logger | logger | Active | requirement-cpp-logger.md |
| requirement-cpp-logger-folder | logger | Active | requirement-cpp-logger-folder.md |
| requirement-cpp-log-retention | logger | Active | requirement-cpp-log-retention.md |
| requirement-cpp-logger-portable | logger | Active | requirement-cpp-logger-portable.md |
| requirement-cpp-output | output | Active | requirement-cpp-output.md |
| requirement-cpp-json | json | Active | requirement-cpp-json.md |
| requirement-cpp-debug-mode | debug | Active | requirement-cpp-debug-mode.md |
| requirement-cpp-parameters | parameters | Active | requirement-cpp-parameters.md |
| requirement-cpp-about | about | Active | requirement-cpp-about.md |
| requirement-cpp-project-structure | structure | Active | requirement-cpp-project-structure.md |
| requirement-cpp-folder | folder | Active | requirement-cpp-folder.md |
| requirement-cpp-tests | tests | Active | requirement-cpp-tests.md |
| requirement-cpp-ant | build | Active | requirement-cpp-ant.md |
| requirement-cpp-ant-coding-style | coding | Active | requirement-cpp-ant-coding-style.md |
| requirement-cpp-build-sh | build | Active | requirement-cpp-build-sh.md |
| requirement-cpp-build-sh-coding-style | coding | Active | requirement-cpp-build-sh-coding-style.md |
| requirement-cpp-dependencies | dependencies | Active | requirement-cpp-dependencies.md |
| requirement-cpp-gradle | build | Active | requirement-cpp-gradle.md |
| requirement-cpp-gradle-coding-style | coding | Active | requirement-cpp-gradle-coding-style.md |
| requirement-cpp-global-installation | install | Active | requirement-cpp-global-installation.md |
| requirement-cpp-local-installation | install | Active | requirement-cpp-local-installation.md |
| requirement-cpp-meson | build | Active | requirement-cpp-meson.md |
| requirement-cpp-meson-coding-style | coding | Active | requirement-cpp-meson-coding-style.md |

The program does not link GTK, WebKit, Lua, or SQLite. That link rule is owned by `requirement-cpp-dependencies.md`.

The OOP row owns the class map. `Logger` is declared in `src/main/cpp/logger.h` and implemented in `src/main/cpp/logger.cpp`. `main` parses the command, then constructs `Logger` with the log name, the log directory, the base directory, quiet, and the clock. That constructor is `requirement-cpp-logger.md` §2.0. The 1.1.3 source writes `Logger logger(PROJECT_NAME, logdir, basedir, quiet, NULL);`. `logger_persistence_storage()` returns the log directory. A new logging behavior is a method on that class before it is a new `logger_*` function. The `logger_*` names that remain forward to that object. The 2026-11-10 run proved the `argc` form only. The five-argument constructor is the current pass.

The logger row owns construction of `Logger`, the six level names, the human line, and the dated file. Construction takes the log name, an optional log directory, an optional base directory, quiet, and a clock. An empty name does not create a directory. Otherwise construction resolves the base, resolves the log directory, creates that directory, and writes one newline when the file can be appended. The level is the enumeration `LoggerOutputLevel`: `INFO`, `WARNING`, `ERROR`, `CRITICAL`, `FATAL`, and `DEBUG`. The line is `[YYYY-MM-DD HH:MM:SS] pid:<pid> [LEVEL]` plus ` @component` when the caller passes a component, then ` :] message`. This product's own events pass `main`. An empty component omits the `@` slot. `ERROR`, `CRITICAL`, and `FATAL` use stderr. The 1.1.3 source does this construction and this level set. Quiet hides every human console line. Debug accepts `show`, `true`, or `1`. `test --home /tmp --log-datestamp 20261003` names `/tmp/.app/cpp-logger/log/cpp-logger-20261003.log`. `test --prefix /tmp` with the same stamp names `/tmp/var/cpp-logger/log/cpp-logger-20261003.log`. `list` prints the names in the selected directory. `show` prints the file for one datestamp.

The log-retention row owns the life of one dated day. A day stays plain for 7 days, becomes one compressed package after that (`tar.gz`, or `zip` on Windows), and is removed after 30 days. `help`, `about`, `list`, and `show` do not log, so they do not perform that maintenance. A log call, including `test`, runs it when the dated path changes. The proof is `TP-CPP-LOG-RETENTION`.

The portable-logger row owns the same notebook for a later project. The later project supplies its own name, version, and component. This product's name is `cpp-logger`, from `PRJ` in `project.ini` and `PROJECT_NAME` in `project.h`. The proof is `TP-CPP-LOGGER-PORTABLE`. `tests/test-cpp-logger-portable.sh` exited 0 on 2026-11-10.

The logger-folder row owns the application directory and the log directory. An explicit directory wins. Otherwise the first match wins: the active conda prefix plus `/.app/cpp-logger`, else the active pyenv virtualenv plus `/.app/cpp-logger`, else `VIRTUAL_ENV` plus `/.app/cpp-logger`. When none of those is set, effective user id 0 uses `/var/cpp-logger` and every other user uses `$HOME/.app/cpp-logger`. Project-log-path is that base plus `/log` unless the log directory was explicit. The project's cache path is a different directory. A normal user uses `$HOME/.cache/cache-cpp-logger/` or `/dev/shm/cache/cache-cpp-logger`. Root or sudo uses `/var/cache/cpp-logger/`. The test command uses `--prefix <dir>` for `<dir>/var/cpp-logger/log` and `--home <dir>` for `<dir>/.app/cpp-logger/log`. It does not read `PREFIX`, `HOME`, or the effective user id. `--prefix` wins when both switches are set.

The output row owns the human-line streams, quiet, `--verbose`, and the debug-mode screen gate. Stdout log lines are hidden unless `--verbose` is set or debug mode is on. Those switches show the lines only when the run is not quiet and not JSON. Quiet hides every human console line, including error, critical, and fatal. The dated file still receives the line. `list` and `show` are not log lines. Quiet does not hide those prints. JSON mode replaces a human line with one object.

The actor row is the light catalog. The columns are Actor, Role, and Subject. There is no Approver column and no approval desk. The operator is the normal user who starts `cpp-logger`.

The about row owns `cpp-logger about`. The page answers three live facts: which program this is, what this computer is, and where the program is running from. The blocks are identity, the host check, and the install location. The verb stays listed in `requirement-cpp-parameters.md`. The page does not append a log line and it does not install. `--json` and `JSON=1` print one object and do not print the page.

The JSON row owns JSON mode. JSON mode implies quiet. Every verb prints JSON: a log event is one object, and `help`, `about`, `list`, and `show` each print one object. The terminal does not show the human text beside that JSON. `--verbose` does not add the human line.

The folder row owns `src/main/cpp`: `main.cpp`, `logger.cpp`, `logger.h`, and `project.h`. That directory is the only ship-unit source root. There is no `src/main/c`.

The project-structure row owns the repository map outside that directory. The source root holds `build.sh`, `build.xml`, `build.gradle`, `meson.build`, `project.sh`, `project.ini`, `tests/`, and `docs/`. `tests/` holds `test-logger-folder.sh`, `test-log-verbs.sh`, `test-run-boundary.sh`, and `test-cpp-about.sh`. `target/`, `build/`, and `.gradle/` are build output. The debug binary is `target/exe/debug/cpp-logger`.

The coding-style row owns linkage, kebab-case, and camel-case. A translation unit is `.cpp` unless `requirement-cpp-folder.md` names a `.c` file. `logger.h` and `project.h` stay headers.

The dependencies row owns the libraries the C++ program links: GLib (`glib-2.0`) and threads. `./build.sh install` is that apt command.

The global-installation row copies `cpp-logger` into `$PREFIX/bin` at mode `0755`. `PREFIX` defaults to `/usr/local`. The local-installation row copies it into `$HOME/.local/bin` at mode `0755`. Those bins are not cpp-log-path. Neither command calls `sudo`. Tests keep writes off the login home with a literal directory under `/tmp`. They do not store a scratch directory in `HOME`. A prefix assignment ends at the next semicolon, and a later delete of `HOME` removes the login home. That check is `requirement-cpp-tests.md`.

`./build.sh` implements `install`, `install-local`, `install-global`, `clean`, and `build`. The script and `project.sh` are `#!/bin/sh`. That interpreter is `requirement-cpp-build-sh.md`. Functions inside `build.sh` use the `build_` prefix and a purpose header. That style is `requirement-cpp-build-sh-coding-style.md`. The primary build verbs stay in `requirement-class-software-dev.md`. GNU Make compiles the same sources. Ant is `requirement-cpp-ant.md`. Gradle is `requirement-cpp-gradle.md`. Meson is `requirement-cpp-meson.md`. `./build.sh` does not gain verbs for those tools.

The Ant row owns `build.xml`. The project name is the `PRJ` key in `project.ini`. The target hosts are Windows, macOS, and Linux with glibc, uClibc, or musl. On Linux, Ant calls `pkg-config --cflags` and `pkg-config --libs` separately, compiles each `.cpp` file with `-std=c++17 -Wall -Wextra -g -O2`, compiles a `.c` file in that same link with `-std=c11 -Wall -Wextra -g -O2`, and writes `target/exe/debug/${PRJ}`. `run` depends on `build`. A generated `project.sh` exports `SRC_C` as `$SRC/main/c` and `SRC_CPP` as `$SRC/main/cpp`.

The Ant coding-style row owns the comment banners, the verb names, and the rule against a hardcoded project name. Well-known verbs are `build`, `compile`, `clean`, `install`, `run`, `prepare`, `make`, `build-windows`, and `build-macos`. Each target uses the same `Purpose` through `Protection Rule` banner. Targets use `${app.name}`.

The Gradle row owns `build.gradle`. The project name is the `PRJ` key in `project.ini`. The target hosts are Windows, macOS, and Linux with glibc, uClibc, or musl. On Linux, Gradle calls `pkg-config --cflags` and `pkg-config --libs` separately, compiles with `-std=c++17 -Wall -Wextra -g -O2`, and writes `target/exe/debug/${PRJ}`. The `run` task depends on `assemble`. A generated `project.sh` exports `SRC_C` and `SRC_CPP`, both as `$SRC/main/cpp`.

The Gradle coding-style row owns the comment banners and the verb names. Well-known verbs are `help`, `tasks`, `properties`, `dependencies`, `clean`, `assemble`, `build`, and `check`. Product tasks are `run`, `scriptVersion`, `installDependencies`, `buildWindows`, and `buildMac`. Each function and product task uses the same `Purpose` through `Protection Rule` banner.

The Meson row owns `meson.build`. It follows the Ant and Gradle host set: Windows, macOS, and Linux with glibc, uClibc, or musl. On Linux, `dependency('glib-2.0')` and `dependency('threads')` split compile flags from link flags. Options are `c_std=c11`, `cpp_std=c++17`, `warning_level=2`, and `buildtype=debugoptimized`. `meson compile -C target/exe/debug` writes `target/exe/debug/${PRJ}`. `run` depends on that executable. A generated `project.sh` exports `SRC_C` as `$SRC/main/c` and `SRC_CPP` as `$SRC/main/cpp`.

The Meson coding-style row owns the `#` comment banners and the verb names. Well-known verbs are `setup`, `compile`, `clean`, `test`, `introspect`, `install`, and `run`. `compile` is the Ant `build` and the Gradle `assemble`. The product target is `run`. The product id literal stays on `project()`.
