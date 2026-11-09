# Requirements registry

Product law for cpp-logger. Rows match files in this folder.

| Key | Area | Status | Path |
|-----|------|--------|------|
| requirement-actor-role-subject | architecture | Active | requirement-actor-role-subject.md |
| requirement-class-software-dev | class | Active | requirement-class-software-dev.md |
| requirement-cpp-coding-style | coding | Active | requirement-cpp-coding-style.md |
| requirement-cpp-logger | logger | Active | requirement-cpp-logger.md |
| requirement-cpp-logger-folder | logger | Active | requirement-cpp-logger-folder.md |
| requirement-cpp-output | output | Active | requirement-cpp-output.md |
| requirement-cpp-json | json | Active | requirement-cpp-json.md |
| requirement-cpp-debug-mode | debug | Active | requirement-cpp-debug-mode.md |
| requirement-cpp-parameters | parameters | Active | requirement-cpp-parameters.md |
| requirement-cpp-project-structure | structure | Active | requirement-cpp-project-structure.md |
| requirement-cpp-folder | folder | Active | requirement-cpp-folder.md |
| requirement-cpp-tests | tests | Active | requirement-cpp-tests.md |
| requirement-cpp-dependencies | dependencies | Active | requirement-cpp-dependencies.md |
| requirement-cpp-global-installation | install | Active | requirement-cpp-global-installation.md |
| requirement-cpp-local-installation | install | Active | requirement-cpp-local-installation.md |

The program does not link GTK, WebKit, Lua, or SQLite. That link rule is owned by `requirement-cpp-dependencies.md`.

The logger row owns `logger_*` in `src/main/cpp/logger.cpp` and `src/main/cpp/logger.h`, the human line `[YYYY-MM-DD HH:MM:SS] pid:<pid> [<LEVEL>] @main :] <message>`, and the dated file that stores it. The well-known levels are `INFO`, `DEBUG`, `WARNING`, `ERROR`, and `FATAL`. `test --home /tmp --log-datestamp 20261003` names `/tmp/.app/cpp-logger/log/cpp-logger-20261003.log`. `test --prefix /tmp` with the same stamp names `/tmp/var/cpp-logger/log/cpp-logger-20261003.log`. `list` prints the names in the selected directory. `show` prints the file for one datestamp.

The logger-folder row owns the application directory and the log directory. Project-base-path is the application directory for the current environment. Root or sudo uses `/var/cpp-logger`. Every other user uses `$HOME/.app/cpp-logger`. Project-log-path is that directory plus `/log`: `/var/cpp-logger/log` or `$HOME/.app/cpp-logger/log`. The project's cache path is a different directory. A normal user uses `$HOME/.cache/cache-cpp-logger/` or `/dev/shm/cache/cache-cpp-logger`. Root or sudo uses `/var/cache/cpp-logger/`. The test command uses `--prefix <dir>` for `<dir>/var/cpp-logger/log` and `--home <dir>` for `<dir>/.app/cpp-logger/log`. It does not read `PREFIX`, `HOME`, or the effective user id. `--prefix` wins when both switches are set.

The output row owns the human-line streams, quiet, `--verbose`, and the debug-mode screen gate. Stdout log lines are hidden unless `--verbose` is set or debug mode is on. Those switches show the lines only when the run is not quiet and not JSON. The human error and fatal lines on stderr stay. `list` and `show` are not log lines, so that gate does not hide them.

The actor row is the light catalog. The columns are Actor, Role, and Subject. There is no Approver column and no approval desk. The operator is the normal user who starts `cpp-logger`.

The JSON row owns JSON mode. JSON mode implies quiet. The terminal shows the JSON object and does not show another product message. `--verbose` does not add the human line.

The folder row owns `src/main/cpp`: `main.cpp`, `logger.cpp`, `logger.h`, and `project.h`. That directory is the only ship-unit source root.

The coding-style row owns linkage, kebab-case, and camel-case. A translation unit is `.cpp` unless `requirement-cpp-folder.md` names a `.c` file. `logger.h` and `project.h` stay headers.

The dependencies row owns the libraries the C++ program links: GLib (`glib-2.0`) and threads. `./build.sh install` is that apt command.

The global-installation row copies `cpp-logger` into `$PREFIX/bin` at mode `0755`. `PREFIX` defaults to `/usr/local`. The local-installation row copies it into `$HOME/.local/bin` at mode `0755`. Those bins are not cpp-log-path. Neither command calls `sudo`. Tests do not use `HOME`. They build one binary and run the test command. That check is `requirement-cpp-tests.md`.

`./build.sh` implements `install`, `install-local`, `install-global`, `clean`, and `build`. The primary build verbs stay in `requirement-class-software-dev.md`. Meson, GNU Make, Apache Ant, and Gradle compile the same sources. `./build.sh` does not gain verbs for those tools.
