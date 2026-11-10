**file**: docs/requirements/requirement-cpp-oop.md
**Status**: Active
**Updated**: 2026-11-11
**Philosophy**: CIAO v2.10.2 / CIAO-Lite (Caution • Intentional • Anti-fragile • Over-engineered / Over-protect)

## 1. Purpose

This file owns the order of implementation for the ship unit. Object-oriented code comes first. A new behavior is a method on a class before it is a new free function. The conversion procedure collects the member variables and the possible member functions before any source move. The line text, the dated file, retention, JSON, and the about page stay on their own requirements. The ship unit is `class Logger`. `main` constructs it at the call site.

### 1.1 Human-facing

**In one sentence:** You add a logging behavior as a method on the logger class, and `main` constructs that class at the call site.

| Box | Meaning | Example |
|-----|---------|---------|
| You / this login | The person who changes the ship unit | You add a method instead of a new free function |
| The other role | The class | It owns quiet, the line, the dated file, list, and show |
| Not this file | The wording of a log line, the retention ages, and the about page | The logger, retention, JSON, and about requirements |

| Includes | Excludes |
|----------|----------|
| The class map, the collected member variables, the collected possible member functions, the rule that a new behavior starts as a method, and the rule that `main` constructs the class at the call site | Line format, folder paths, retention ages, JSON shape, about-page text, and compile flags |

| Surface | What you open | What for |
|---------|---------------|----------|
| `src/main/cpp/logger.h` | The class declaration | The type the rest of the program uses |
| `src/main/cpp/logger.cpp` | The method bodies | The behavior those methods perform |
| `src/main/cpp/main.cpp` | The entry | The one construction of the class |

| You do… | What it means | What you type |
|---------|---------------|---------------|
| Add a logging behavior | The method exists on the class before any new free function | A method on `Logger` |
| Start the program | `main` parses the command, then constructs the class with the log name, the directories, quiet, and the clock | `Logger logger(logname, logdir, basedir, is_quiet, clock);` |
| Convert the present logger | Collect the members and the possible methods, then move that set | The tables in §2.3 |

## 2. Core rules

1. Object-oriented code is the first implementation shape. A new behavior of the program is a method on a class before it is a new free function. A coding-style note does not replace this file.
2. The allowed end state is the class map in this file. A pile of free functions may be the present call surface. It is not the allowed end state.
3. One class owns one job. A class with a single method is forbidden. The entry function stays in its own file. It constructs the class. It does not define that class's methods.
4. The site that needs the object writes the class name and constructs it there. A function or a method whose only job is to return a new object is not in the map. The class can be constructed without that helper.
5. Line text, folder paths, retention, JSON, and the about page stay on the requirements that already own them. This file names the type. It does not copy those rules.
6. The class stays in the source. `main` constructs it at the call site. Mark TP-CPP-OOP pass only after `tests/test-cpp-oop.sh` has exited 0 on that source.
7. The conversion procedure collects the member variables and the possible member functions first. The move uses that collected set. A data member or a method outside the collection is not part of the move.

### 2.1 Implementation notes (this project)

The ship unit is `src/main/cpp/logger.h`, `src/main/cpp/logger.cpp`, `src/main/cpp/main.cpp`, and `src/main/cpp/project.h`. `logger.h` declares `class Logger`. The allowed constructor is the five-argument form in `requirement-cpp-logger.md` §2.0. `main.cpp` parses `argc` and `argv`, then constructs `Logger` with the log name, the log directory, the base directory, quiet, and the clock. It declares no second type. The `logger_*` names stay in `extern "C"` and forward to that object. The 1.1.2 source writes `Logger logger(PROJECT_NAME, logdir, basedir, quiet, NULL);`.

The target type is `Logger`. The declaration lives in `logger.h`. The method bodies live in `logger.cpp`. `main.cpp` constructs it at the call site:

```cpp
Logger logger(logname, logdir, basedir, is_quiet, clock);
```

The 1.1.2 call in this position is `Logger logger(PROJECT_NAME, logdir, basedir, quiet, NULL);`. The 1.1.1 call `Logger logger(argc, argv);` is the earlier record.

| Type | File | Job |
|------|------|-----|
| `Logger` | `logger.h`, `logger.cpp` | The members and the methods collected in §2.3. That set covers quiet, debug, JSON, the human line, the dated file, retention, `list`, `show`, and the host lines the about page already reads from this unit |
| `main` | `main.cpp` | Constructs `Logger` and calls its methods. It does not declare a second type |

`project.h` stays the name and version macros. It is not a class.

`logger_*` may remain only as forwarding wrappers after the implement order. A new behavior does not start as a new `logger_*` function. Spelling of a C linkage name stays in `requirement-cpp-coding-style.md`.

### 2.2 Why this requirement exists

- **CIAO Principle 2** (https://github.com/cloudgen/ciao): the class job is named before the source moves.
- **CIAO Principle 5** (https://github.com/cloudgen/ciao): one type owns output. The line, the path, and the about page do not grow a second owner here.

### 2.3 Conversion procedure

The move followed these steps in order. This section is the collection the source uses.

1. **Collect the member variables.** Read every file-scope `static` variable in `src/main/cpp/logger.cpp`. Each name below becomes a non-static data member of `Logger`. The constructor sets each member to the initial value in the table. The two character arrays start zero-filled, matching their file-scope lifetime today. `LoggerOutputLevel` stays the enum beside the class. The macros in `project.h` stay macros. `main` holds none of these variables.

| Name | Type | Initial value |
|------|------|----------------|
| `logger_quiet_mode` | `bool` | `false` |
| `logger_json_mode` | `bool` | `false` |
| `debug_env_enabled` | `bool` | `false` |
| `logger_verbose` | `int` | `0` |
| `logger_folder` | `char[4096]` | zero-filled |
| `logger_log_path` | `char[4096]` | zero-filled |
| `logger_purpose` | `int` | `0` |
| `logger_prefix_arg` | `const char *` | `NULL` |
| `logger_home_arg` | `const char *` | `NULL` |
| `logger_datestamp_arg` | `const char *` | `NULL` |
| `logger_saw_quiet` | `int` | `0` |
| `logger_saw_json` | `int` | `0` |
| `logger_saw_debug` | `int` | `0` |
| `logger_saw_verbose` | `int` | `0` |
| `logger_pending_page` | `int` | `0` |
| `logger_retention_done` | `int` | `0` |
| `logger_maintain_busy` | `int` | `0` |

The 17 names above are private non-static members. A file-scope `static` added in `logger.cpp` after this table is collected into this table in the same change, before it becomes a member.

`logger_active` is one public static member in the same collection. A C caller, including `tests/cpp-logger-retain-driver.cpp`, has no object parameter. The 1.1.1 constructor stores `this` there when `argc >= 0`. A 1.1.1 constructor call with `argc < 0` does not store the object, so the retain driver can run retention without parsing argv or appending a log. That `argc` rule is the 1.1.1 record. The allowed constructor is step 4 and does not take `argc`. An empty log name is the return in `requirement-cpp-logger.md` §2.0: no directory and no file write. The pointer is not a function that returns a `Logger`.

| Name | Type | Initial value |
|------|------|----------------|
| `logger_active` | `Logger *` | `NULL` |

2. **Collect the possible member functions.** Read every function defined in `src/main/cpp/logger.cpp`. The collection below is the 1.1.1 set. In that move, `logger_output_init` became the constructor recorded in the table. The allowed constructor is step 4. Each other function with external linkage becomes a public method. Each file-static function becomes a private method. `logger_log` keeps `va_list` by value and keeps `va_copy` before `g_strdup_vprintf`. `logger_maintain_directory` stays callable under that C name, because `tests/cpp-logger-retain-driver.cpp` calls it. After the move, that C name forwards to the method.

Constructor:

| Present function | Becomes |
|------------------|---------|
| `logger_output_init(int argc, char **argv)` | `Logger(int argc, char **argv)` |

That row records the 1.1.1 move. It is not the allowed constructor. The allowed constructor is step 4.

Public methods:

| Present function |
|------------------|
| `logger_is_test_verb(void)` |
| `logger_set_quiet(bool quiet)` |
| `logger_get_quiet(void)` |
| `logger_get_debug(void)` |
| `logger_log(LoggerOutputLevel level, const char *format, va_list ap)` |
| `logger_info(const char *format, ...)` |
| `logger_warn(const char *format, ...)` |
| `logger_error(const char *format, ...)` |
| `logger_fatal(const char *format, ...)` |
| `logger_debug(const char *format, ...)` |
| `logger_maintain_directory(const char *folder, const char *active_basename, const char *today_yyyymmdd)` |
| `logger_print_help(void)` |
| `logger_print_about(void)` |

Private methods:

| Present function |
|------------------|
| `logger_json_escape(const char *text)` |
| `logger_command_name(void)` |
| `logger_die(const char *message)` |
| `logger_join_base(char *out, size_t out_size, const char *head, const char *tail)` |
| `logger_datestamp_ok(const char *text)` |
| `logger_apply_base(const char *base, const char *stamp)` |
| `logger_set_purpose(int purpose)` |
| `logger_list_emit(GSList *names)` |
| `logger_filename_cmp(gconstpointer left, gconstpointer right)` |
| `logger_list_log_files(void)` |
| `logger_show_log_file(void)` |
| `logger_take_value(int argc, char **argv, int *index, const char *flag)` |
| `logger_log_found_switches(void)` |
| `logger_level_name(LoggerOutputLevel level)` |
| `logger_stderr_level(LoggerOutputLevel level)` |
| `logger_emit_chronicle(LoggerOutputLevel level, const char *message)` |
| `logger_unlink_regular(const char *path)` |
| `logger_retention_join(char *out, size_t out_size, const char *folder, const char *name)` |
| `logger_stamp_to_date(const char *stamp, GDate *date)` |
| `logger_retention_stamp(const char *name, char stamp[9])` |
| `logger_spawn_ok(gchar **argv)` |
| `logger_compress_day(const char *folder, const char *stamp, const char *plain_name)` |
| `logger_maintain_one(const char *folder, const char *active_basename, const GDate *today, const char *stamp)` |
| `logger_maintain_body(const char *folder, const char *active_basename, const char *today_yyyymmdd)` |
| `logger_append_log_file(LoggerOutputLevel level, const char *message)` |
| `logger_about_copy(char *out, size_t out_size, const char *value)` |
| `logger_about_probe(char *out, size_t out_size, const char *path, const char *version)` |

`logger_filename_cmp` is a private static method. `g_slist_insert_sorted` takes a function pointer with no object parameter.

The collection is 1 constructor, 13 public methods, 26 private non-static methods, and 1 private static method. A function added in `logger.cpp` after these tables is collected into the matching table in the same change. A new behavior after the move is a method on `Logger` first. That later method is not invented during this collection. `logger_critical` is required by `requirement-cpp-logger.md` and is not in the public-method table above. The constructor row is the 1.1.1 record. Step 4 is the allowed constructor.

3. **Declare the class.** `logger.h` declares `class Logger` with the collected members and the collected methods. `logger.cpp` holds the method bodies. The present `logger_*` names remain only as wrappers that forward to that object. Unbound `logger_maintain_directory` constructs `Logger cold(-1, NULL)` and calls the method on that object.

4. **Construct at the entry.** `main.cpp` parses the command line, then writes the five-argument construction from `requirement-cpp-logger.md` §2.0. It declares no second type. It declares no function whose only job is to return a `Logger`. `Logger(int argc, char **argv)` is the 1.1.1 constructor. It is not this step.

5. **Leave the other product rules where they are.** Line text, folder paths, retention, JSON, and the about page stay on the requirements that already own them.

## Under command line for normal user only

On Termux, Git Bash, Windows cmd, or the same class, the person runs the program as the normal user. This file adds no administrator command, no `sudo`, no `apt`, and no dedicated system user.

## 3. Design principles

- **Caution**: The present `logger_*` names forward to the object. The sentences, the exit codes, and `va_copy` stay.
- **Intentional**: The class is the first place a new behavior is written.
- **Anti-fragile**: `main` constructs `Logger` at the call site, so the type is visible in the entry.
- **Over-protect**: A helper whose only job is to construct `Logger` is not added.

## 4. Verification

| TP-ID | Pass |
|-------|------|
| TP-CPP-OOP | `logger.h` declares `class Logger`. `main.cpp` constructs it with the five arguments in `requirement-cpp-logger.md` §2.0 and does not declare a second type. A function whose only job is to return a `Logger` is absent. The present `logger_*` names, if they remain, forward to that object. The 2026-11-10 run proved `Logger logger(argc, argv);` only. The 2026-11-11 run proves the five-argument constructor |

### Design-time verification

| TP-ID | Suite | Status |
|-------|-------|--------|
| TP-CPP-OOP | `tests/test-cpp-oop.sh` | pass |

**Matrix:** `docs/reviews/requirement-test-matrix.md`
**Map:** `docs/reviews/test-plan.md`

The collection in §2.3 is the procedure record of the 1.1.1 move. On 2026-11-10 `tests/test-cpp-oop.sh` exited 0 for `Logger logger(argc, argv);`. That run does not prove the five-argument constructor. On 2026-11-11 the same script exits 0 on `Logger logger(PROJECT_NAME, logdir, basedir, quiet, NULL);`. That run is the current pass.

## 5. Protection rule

Future editors must not add a new `logger_*` function for a behavior that does not yet have a method on `Logger`, must not add a function whose only job is to construct `Logger`, must not split `Logger` into a class with a single method, must not mark TP-CPP-OOP pass before `main` constructs `Logger`, and must not move source while a file-scope `static` or a function in `logger.cpp` is missing from §2.3. The move uses the collected members and the collected functions. Line text stays in `requirement-cpp-logger.md`. Retention stays in `requirement-cpp-log-retention.md`. JSON stays in `requirement-cpp-json.md`. The about page stays in `requirement-cpp-about.md`.

## 6. Related artifacts

| Artifact | Role |
|----------|------|
| `docs/requirements/index.md` | Registry |
| `src/main/cpp/logger.h` | The class declaration |
| `src/main/cpp/logger.cpp` | The method bodies |
| `src/main/cpp/main.cpp` | The construction site |
| `tests/test-cpp-oop.sh` | The class, the call site, and the forwarding names |
| `docs/requirements/requirement-cpp-coding-style.md` | Spelling and the present `extern "C"` names |
| `docs/requirements/requirement-cpp-logger.md` | The line and the `logger_*` behavior this class owns |
| `docs/reviews/test-plan.md` | TP-CPP-OOP |
| `docs/reviews/requirement-test-matrix.md` | This requirement to TP-CPP-OOP |
| `tests/cpp-logger-retain-driver.cpp` | Calls `logger_maintain_directory`, so that C name keeps forwarding after the move |

## Terminologies

### Avoid-factory-create-class

**Definition:** Avoid-factory-create-class is the construct rule that an object is created by writing the class name at the site that needs that object. A factory is a function or a method whose job is to instantiate a class. The class can be created without that factory. The factory does not add a way to create the object that the call site lacks. It hides the class name behind a second home.

**Human daily-life explanation:** You pick up the thing itself. You do not hand the job to a clerk whose only task is to pick it up and hand it back. The class already knows how to become an object. Writing the class name at the site is that step. A factory is the clerk.

**Daily-life example:** You want a kettle. You take a kettle off the shelf. You do not fill out a slip that a back-room clerk uses to fetch the same kettle and pass it through a window. That slip is a factory. The kettle was already on the shelf.

## 7. Status history

| Date | Status | Notes |
|------|--------|-------|
| 2026-11-10 | Active | OOP-first class map. Source not moved. TP-CPP-OOP stays todo |
| 2026-11-10 | Active | Conversion procedure collects 17 members and 41 possible member functions from `logger.cpp`. Source not moved. TP-CPP-OOP stays todo |
| 2026-11-10 | Active | `class Logger` is in the ship unit. `logger_active` is the static pointer for C callers. `logger_filename_cmp` is a private static method. TP-CPP-OOP stays todo until `tests/test-cpp-oop.sh` exits 0 |
| 2026-11-10 | Active | `tests/test-cpp-oop.sh` exited 0. The seven existing suites exited 0. TP-CPP-OOP is pass for `Logger logger(argc, argv);` |
| 2026-11-11 | Active | The allowed constructor is the five-argument ChronicleLogger form in `requirement-cpp-logger.md` §2.0. The 1.1.1 call `Logger logger(argc, argv);` is not that constructor. TP-CPP-OOP returns to todo |
| 2026-11-11 | Active | Release 1.1.2 writes the five-argument constructor. `tests/test-cpp-oop.sh` exits 0 on that call. TP-CPP-OOP is pass |

**Last updated**: 2026-11-11
**Owner**: OOP
