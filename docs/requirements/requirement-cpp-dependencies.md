**file**: docs/requirements/requirement-cpp-dependencies.md
**Status**: Active
**Updated**: 2026-11-10
**Philosophy**: CIAO v2.10.2 / CIAO-Lite (Caution • Intentional • Anti-fragile • Over-engineered / Over-protect)

## 1. Purpose

This file owns the libraries the C++ program compiles and links against, and the apt packages that provide them. `./build.sh install` is the command that requests the apt packages. `./build.sh build` and `meson.build` both request the same link set.

### 1.1 Human-facing

**In one sentence:** You install GLib, and both builds link `glib-2.0`.

| Box | Meaning | Example |
|-----|---------|---------|
| You / this login | The person who prepares the machine | You run `./build.sh install` |
| The other role | The link line | It asks pkg-config for GLib |
| Not this file | Where the built binary is installed | `requirement-cpp-global-installation.md` and `requirement-cpp-local-installation.md` |

| Includes | Excludes |
|----------|----------|
| The link modules, the apt packages, and pthread | A second package list inside the class file |

| Surface | What you open | What for |
|---------|---------------|----------|
| `./build.sh` | `build_install_deps` and the `pkg-config` call in `build_link` | Apt packages and the link set |
| `meson.build` | `dependency()` entries | The same link set |

| You do… | What it means | What you type |
|---------|---------------|---------------|
| Install the libraries | Apt packages below | `./build.sh install` |

## 2. Core rules

The link set is exactly these modules. `./build.sh build` passes them to `pkg-config --cflags --libs`. `meson.build` declares each one with `dependency()`, `required: true`.

| Library | pkg-config / Meson name | Apt package |
|---------|-------------------------|-------------|
| GLib | `glib-2.0` | `libglib2.0-dev` |
| POSIX threads | Meson name `threads`; the simple link line adds `-pthread` | comes with `build-essential` |

The ship unit does not link GTK, WebKitGTK, GObject, GIO, SQLite, or Lua. It does not install sol2 headers.

`./build.sh install` also installs `build-essential` and `pkg-config`. It does not copy the built binary.

The second build needs `meson` and `ninja-build` on the developer machine. `./build.sh install` does not install those two packages.

`project.sh` `REQUIRED_LIBS` names `glib-2.0`. `./build.sh` reads `project.sh` with `.`. The `pkg-config` call inside `build_link` uses the table above.

Package versions are the distro packages for these names. This file has no version pins and no lockfile.

### 2.1 Implementation notes (this project)

Simple build, the `pkg-config` arguments:

```text
glib-2.0
```

The link line appends `-pthread`.

`meson.build` repeats `glib-2.0` and adds `dependency('threads')`.

### 2.2 Why this requirement exists

- **CIAO Principle 5** (https://github.com/cloudgen/ciao): one dependency list. Both builds consume the table in this file.
- **CIAO Principle 1**: a missing `libglib` fails at configure or link time, on the module name in the table.

## 3. Design principles

- **Caution**: a library joins the link set only when product code includes it.
- **Intentional**: a new linked library is added to the table, to the `pkg-config` call, and to `meson.build` in the same change.
- **Anti-fragile**: GLib stays named even though the console writer is the only caller.

## 4. Verification

| TP-ID | Pass |
|-------|------|
| TP-CPP-DEPENDENCIES | `pkg-config --exists glib-2.0` succeeds. `build.sh` and `meson.build` both request that module and do not request `gtk+-3.0` or `webkit2gtk-4.1` |

### Design-time verification

| TP-ID | Suite | Status |
|-------|-------|--------|
| TP-CPP-DEPENDENCIES | `tests/test-cpp-builds.sh` | pass |

**Matrix:** `docs/reviews/requirement-test-matrix.md`
**Map:** `docs/reviews/test-plan.md`

## 5. Protection rule

Future editors must not add GTK, WebKitGTK, Lua, SQLite, or sol2 to this table unless a new Active requirement owns that library, and must not copy the package list back into `requirement-class-software-dev.md`.

## 6. Related artifacts

| Artifact | Role |
|----------|------|
| `docs/requirements/index.md` | Registry |
| `build.sh` | Apt list and `pkg-config` call |
| `meson.build` | `dependency()` calls |
| `project.ini` | `REQUIRED_PACKAGES` and `LIB_VERSIONS` |

**Last updated**: 2026-11-10
**Owner**: Dependencies
