**file**: docs/requirements/requirement-cpp-oop.md
**Status**: Active
**Updated**: 2026-11-11
**Philosophy**: CIAO v2.10.2 / CIAO-Lite (Caution • Intentional • Anti-fragile • Over-engineered / Over-protect)

## 1. Purpose

This file owns the order of implementation for the ship unit. Object-oriented code comes first. A new behavior is a method on a class before it is a new free function. The conversion procedure collects the member variables and the possible member functions before any source move. The line text, the dated file, retention, JSON, and the about page stay on their own requirements. The ship unit is `class About`, `class Logger`, `class Invocation`, `class AboutCli`, and `class Help`. Each class has its own header and its own translation unit. `main` constructs `About` and calls `run`. `About` is the runtime source of the name, the version, the paths, and the flags. `Logger` binds that object. `Invocation`, `AboutCli`, and `Help` bind `Logger` and are loggable.

### 1.1 Human-facing

**In one sentence:** You add a logging behavior as a method on the logger class, and `main` constructs `About`, which binds that logger after the flags are known.

| Box | Meaning | Example |
|-----|---------|---------|
| You / this login | The person who changes the ship unit | You add a method instead of a new free function |
| The other role | `About`, then `Logger` | `About` owns the name, the version, the paths, and the flags. `Logger` records the line |
| Not this file | The wording of a log line, the retention ages, and the about page | The logger, retention, JSON, and about requirements |

| Includes | Excludes |
|----------|----------|
| The class map, the collected member variables, the collected possible member functions, the rule that a new behavior starts as a method, and the rule that `main` constructs `About` at the call site | Line format, folder paths, retention ages, JSON shape, about-page text, and compile flags |

| Surface | What you open | What for |
|---------|---------------|----------|
| `src/main/cpp/logger.h` | The class declaration | The type the rest of the program uses |
| `src/main/cpp/logger.cpp` | The method bodies | The behavior those methods perform |
| `src/main/cpp/main.cpp` | The entry | The one construction of `About` |

| You do… | What it means | What you type |
|---------|---------------|---------------|
| Add a logging behavior | The method exists on the class before any new free function | A method on `Logger` |
| Start the program | `main` constructs `About`. `About` parses, binds `Logger`, routes one verb, and finishes | `About about;` then `about.run(argc, argv)` |
| Convert the present logger | Collect the members and the possible methods, then move that set | The tables in §2.3 |
| Bind the logger | The logger reads the name, the paths, and the flags from `About` | `Logger logger(about, clock);` |

## 2. Core rules

1. Object-oriented code is the first implementation shape. A new behavior of the program is a method on a class before it is a new free function. A coding-style note does not replace this file.
2. The allowed end state is the class map in this file. A pile of free functions may be the present call surface. It is not the allowed end state.
3. One class owns one job. A class that owns one job keeps that job even when the class has one method. Folding a second job into a class that already owns a different job is forbidden. The entry function stays in its own file. It constructs the class. It does not define that class's methods.
4. The site that needs the object writes the class name and constructs it there. A function or a method whose only job is to return a new object is not in the map. The class can be constructed without that helper.
5. Line text, folder paths, retention, JSON, and the about page stay on the requirements that already own them. This file names the type. It does not copy those rules.
6. The classes stay in the source. `main` constructs `About` at the call site. Mark TP-CPP-OOP pass only after `tests/test-cpp-oop.sh` has exited 0 on that source.
7. The conversion procedure collects the member variables and the possible member functions first. The move uses that collected set. A data member or a method outside the collection is not part of the move.
8. When a conversion cannot finish in one pass, a function that still does more than one job is an intermediate step. The same record names each remaining job and the class that should own it. The next pass starts from that naming. A record that stops on the intermediate function and omits those classes is not finished.

### 2.1 Implementation notes (this project)

The ship unit is `src/main/cpp/about.h`, `src/main/cpp/about.cpp`, `src/main/cpp/aboutcli.h`, `src/main/cpp/aboutcli.cpp`, `src/main/cpp/help.h`, `src/main/cpp/help.cpp`, `src/main/cpp/invocation.h`, `src/main/cpp/invocation.cpp`, `src/main/cpp/logger.h`, `src/main/cpp/logger.cpp`, `src/main/cpp/main.cpp`, and `src/main/cpp/project.h`. `about.h` declares `class About`. `aboutcli.h` declares `class AboutCli`. `help.h` declares `class Help`. `invocation.h` declares `class Invocation`. `logger.h` declares `class Logger`. `main.cpp` constructs `About` and calls `run`. It declares no second type. `About` loads the macros in `project.h` and is the runtime source of the name, the version, the paths, and the flags. It parses the flags, then the command. It constructs `Logger` after parse. A reader flag is already set, so `help`, `about`, `list`, and `show` do not create a directory. `Logger` records. It does not choose the verb and it does not exit the process for a successful reader verb. `Invocation`, `AboutCli`, and `Help` bind `Logger`. `AboutCli` and `Help` print. Under debug mode, a public call and a major member change are debug records. A reader record does not create a directory and does not run retention. The `logger_*` names stay in `extern "C"`. The logging names forward to `Logger`. `logger_print_about()` constructs `AboutCli` and binds that logger. The allowed `Logger` constructor is `Logger(About &about, LoggerClock clock)` in `requirement-cpp-logger.md` §2.0.

The entry is:

```cpp
About about;
return about.run(argc, argv);
```

The 1.1.1 call `Logger logger(argc, argv);` and the 1.1.2 call `Logger logger(PROJECT_NAME, logdir, basedir, quiet, NULL);` are earlier records. The current call is `Logger logger(about, clock);` after `About` has parsed.

| Type | File | Job |
|------|------|-----|
| `About` | `about.h`, `about.cpp` | The name, the version, the paths, and the flags. Initialize, parse, bind `Logger`, route one verb, and finish. `main` does not declare this type |
| `AboutCli` | `aboutcli.h`, `aboutcli.cpp` | The human about page and its JSON object. It binds `Logger`. Persistence storage is the application directory. The log folder is that directory plus `/log` |
| `Help` | `help.h`, `help.cpp` | The human usage page and its JSON object. It binds `Logger` |
| `Invocation` | `invocation.h`, `invocation.cpp` | The command of this run. Page, purpose, and complete. It binds `Logger` |
| `Logger` | `logger.h`, `logger.cpp` | The human line, the dated file, retention, `list`, and `show`. It binds `About`. `logger_persistence_storage()` returns the log directory |
| `main` | `main.cpp` | Constructs `About` and calls `run`. It does not declare a second type |

`project.h` stays the compile-time macros. `About` loads them. `project.h` is not a class.

`About::parse` reads the tokens and the values and stores them on `About`. `About::run` chooses the one verb. The path join for a test-purpose command stays on `requirement-cpp-logger-folder.md`.

`logger_*` may remain only as forwarding wrappers after the implement order. A new behavior does not start as a new `logger_*` function. Spelling of a C linkage name stays in `requirement-cpp-coding-style.md`.

### 2.2 Why this requirement exists

- **CIAO Principle 2** (https://github.com/cloudgen/ciao): the class job is named before the source moves.
- **CIAO Principle 5** (https://github.com/cloudgen/ciao): `About` owns the name, the version, the paths, and the flags. `AboutCli` owns the about page. `Help` owns the usage page. `Logger` owns the log line.

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
| `logger_persistence_storage(void)` |
| `logger_log(LoggerOutputLevel level, const char *format, va_list ap)` |
| `logger_info(const char *format, ...)` |
| `logger_warn(const char *format, ...)` |
| `logger_error(const char *format, ...)` |
| `logger_fatal(const char *format, ...)` |
| `logger_debug(const char *format, ...)` |
| `logger_maintain_directory(const char *folder, const char *active_basename, const char *today_yyyymmdd)` |
| `logger_print_help(void)` |
| `logger_get_json(void)` |
| `logger_record_debug(const char *component, const char *message)` |

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

`logger_filename_cmp` is a private static method. `g_slist_insert_sorted` takes a function pointer with no object parameter.

The collection is 1 constructor, 14 public methods, 24 private non-static methods, and 1 private static method. That count is the 1.1.1 procedure record. The live public table adds `logger_record_debug(const char *component, const char *message)` for a loggable debug line. `logger_get_json(void)` replaces `logger_print_about(void)` in the public table. The two about helpers left the private table. `logger_persistence_storage(void)` returns the log directory. A function added in `logger.cpp` after these tables is collected into the matching table in the same change. A new logging behavior after the move is a method on `Logger` first. The about page is `AboutCli::printPage`. The usage page is `Help::printPage`. `logger_critical` is required by `requirement-cpp-logger.md` and is not in the public-method table above. The constructor row in the table is the 1.1.1 record. The allowed constructor is `Logger(About &about, LoggerClock clock)`.

3. **Declare the class.** `logger.h` declares `class Logger` with the collected members and the collected methods. `logger.cpp` holds the method bodies. The present `logger_*` names remain only as wrappers that forward to that object. Unbound `logger_maintain_directory` builds an `About` whose name is empty, constructs `Logger cold(coldAbout, NULL)`, and calls the method on that object. The empty name returns before a directory is created and does not store `logger_active`.

4. **Construct at the entry.** `main.cpp` constructs `About` and calls `run`. It declares no second type. It declares no function whose only job is to return a `Logger`. `Logger(int argc, char **argv)` is the 1.1.1 constructor. The five-argument form is the 1.1.2 constructor. The allowed constructor is `Logger(About &about, LoggerClock clock)`.

5. **Leave the other product rules where they are.** Line text, folder paths, retention, JSON, and the about page stay on the requirements that already own them.

## Under command line for normal user only

On Termux, Git Bash, Windows cmd, or the same class, the person runs the program as the normal user. This file adds no administrator command, no `sudo`, no `apt`, and no dedicated system user.

## 3. Design principles

- **Caution**: The present `logger_*` names forward to the object. The sentences, the exit codes, and `va_copy` stay.
- **Intentional**: The class is the first place a new behavior is written.
- **Anti-fragile**: `main` constructs `About` at the call site, so the program type is visible in the entry.
- **Over-protect**: A helper whose only job is to construct `Logger` is not added.

## 4. Verification

| TP-ID | Pass |
|-------|------|
| TP-CPP-OOP | `logger.h` declares `class Logger` once. `about.h` declares `class About` once. `aboutcli.h` declares `class AboutCli` once. `help.h` declares `class Help` once. `main.cpp` constructs `About` and calls `run`, and does not declare a second type. The allowed logger constructor is `Logger(About &about, LoggerClock clock)`. A function whose only job is to return a `Logger` is absent. The logging `logger_*` names forward to `Logger`. `logger_print_about()` constructs `AboutCli`. `invocation.h` declares `class Invocation` once. `Invocation`, `AboutCli`, and `Help` bind `Logger`. `AboutCli` prints the page. `Help` prints usage. Under debug mode, `help` records `Help::printPage` and does not create a directory. `help --json` with debug stays one object. The 2026-11-10 run proved `Logger logger(argc, argv);` only. The earlier 2026-11-11 run proved the five-argument constructor. The current run proves `About about;` and `return about.run(argc, argv);` |

### Design-time verification

| TP-ID | Suite | Status |
|-------|-------|--------|
| TP-CPP-OOP | `tests/test-cpp-oop.sh` | pass |

**Matrix:** `docs/reviews/requirement-test-matrix.md`
**Map:** `docs/reviews/test-plan.md`

The collection in §2.3 is the procedure record of the 1.1.1 move. On 2026-11-10 `tests/test-cpp-oop.sh` exited 0 for `Logger logger(argc, argv);`. That run does not prove the current constructor. On 2026-11-11 an earlier run of the same script exited 0 on `Logger logger(PROJECT_NAME, logdir, basedir, quiet, NULL);`. That run is the five-argument record. The current run exits 0 on `About about;` and `return about.run(argc, argv);`, with `Logger(About &about, LoggerClock clock)`. That run is the current pass.

## 5. Protection rule

Future editors must not add a new `logger_*` function for a behavior that does not yet have a method on `Logger`, must not add a function whose only job is to construct `Logger`, must not peel one logging method of `Logger` into a class that only wraps that method, must not mark TP-CPP-OOP pass before `main` constructs `About`, and must not move source while a file-scope `static` or a function in `logger.cpp` is missing from §2.3. The move uses the collected members and the collected functions. `About` owns the name, the version, the paths, and the flags. `Logger` binds `About`. `Invocation`, `AboutCli`, and `Help` bind `Logger`. A second copy of those facts on `Logger` is not added. The path join stays on `requirement-cpp-logger-folder.md`. Line text stays in `requirement-cpp-logger.md`. Retention stays in `requirement-cpp-log-retention.md`. JSON stays in `requirement-cpp-json.md`. The about page stays in `requirement-cpp-about.md`. The usage page stays in `Help`.

## 6. Related artifacts

| Artifact | Role |
|----------|------|
| `docs/requirements/index.md` | Registry |
| `src/main/cpp/logger.h` | The class declaration |
| `src/main/cpp/logger.cpp` | The `Logger` method bodies. `logger_print_about()` constructs `AboutCli` |
| `src/main/cpp/about.h` | `class About`, the runtime source of the name, the version, the paths, and the flags |
| `src/main/cpp/about.cpp` | `About::parse` and `About::run` |
| `src/main/cpp/aboutcli.h` | `class AboutCli` |
| `src/main/cpp/aboutcli.cpp` | The about page and its JSON object |
| `src/main/cpp/help.h` | `class Help` |
| `src/main/cpp/help.cpp` | The usage page and its JSON object |
| `src/main/cpp/invocation.h` | `class Invocation` |
| `src/main/cpp/invocation.cpp` | The command of this run |
| `src/main/cpp/main.cpp` | The `About` construction site |
| `tests/test-cpp-oop.sh` | The class, the call site, and the forwarding names |
| `docs/requirements/requirement-cpp-coding-style.md` | Spelling and the present `extern "C"` names |
| `docs/requirements/requirement-cpp-logger.md` | The line and the `logger_*` behavior this class owns |
| `docs/reviews/test-plan.md` | TP-CPP-OOP |
| `docs/reviews/requirement-test-matrix.md` | This requirement to TP-CPP-OOP |
| `tests/cpp-logger-retain-driver.cpp` | Calls `logger_maintain_directory`, so that C name keeps forwarding after the move |

## Terminologies

### Loggable

**Definition:** A loggable component binds the logger. Under debug mode, a major change of a member variable and an invocation of a member function are recorded as a debug log message.

**Human daily-life explanation:** A loggable tool is clipped to the household notebook. On an ordinary day the checking page stays closed, and using the tool writes nothing. When that page is open, using one of the tool’s actions and changing one of its important settings each leave a note. A small step that only helps that action finish does not get its own note.

**Daily-life example:** The kitchen timer is clipped to the notebook. You open the checking page, press start, and the notebook gets “start pressed.” You change the minutes from 5 to 10, and the notebook gets “minutes 10.” You close the checking page, press start again, and the timer runs with no new note.

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
| 2026-11-11 | Active | Release 1.1.3 adds `logger_persistence_storage`. The same script exits 0 for quiet errors, the log-directory return, and the debug tokens. TP-CPP-OOP stays pass |
| 2026-11-11 | Active | `class About` is `about.h` and `about.cpp`. The about page left `Logger`. `tests/test-cpp-oop.sh` exits 0. TP-CPP-OOP stays pass |
| 2026-11-11 | Active | `logger_parse_invocation` is an intermediate step. The next classes are `ParaParser` and `Invocation`. They are not written. TP-CPP-OOP stays pass |
| 2026-11-11 | Active | `About` is the runtime source of the name, the version, the paths, and the flags. The about page is `AboutCli`. The usage page is `Help`. `Logger`, `AboutCli`, and `Help` bind `About`. `tests/test-cpp-oop.sh` exits 0. TP-CPP-OOP stays pass |
| 2026-11-11 | Active | `Invocation`, `AboutCli`, and `Help` bind `Logger`. Under debug mode a public call and a major member change are debug records. A reader record does not create a directory. `help --json` with debug stays one object. `tests/test-cpp-oop.sh`, `tests/test-cpp-about.sh`, `tests/test-cpp-logger-portable.sh`, `tests/test-log-verbs.sh`, `tests/test-run-boundary.sh`, `tests/test-logger-folder.sh`, and `tests/test-cpp-log-retention.sh` exit 0. TP-CPP-OOP stays pass |
| 2026-11-11 | Active | Release 1.2.0 prints `cpp-logger 1.2.0`. The same seven suites exit 0 on that triple. TP-CPP-OOP stays pass |

**Last updated**: 2026-11-11
**Owner**: OOP
