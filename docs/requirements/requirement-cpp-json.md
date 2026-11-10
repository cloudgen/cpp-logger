**file**: docs/requirements/requirement-cpp-json.md
**Status**: Active
**Updated**: 2026-11-10
**Philosophy**: CIAO v2.10.2 / CIAO-Lite (Caution • Intentional • Anti-fragile • Over-engineered / Over-protect)

## 1. Purpose

This file owns JSON mode. JSON mode implies quiet. Every CLI verb prints JSON and does not print its human text beside that JSON. A log event is one object per event. `help`, `about`, `list`, and `show` each print one object. `test` prints one object per log event it emits.

### 1.1 Human-facing

**In one sentence:** You pass `--json` or `JSON=1`, and every verb prints JSON with no human page beside it.

| Box | Meaning | Example |
|-----|---------|---------|
| You / this login | The person collecting a machine log | You pass `--json` |
| The other role | `logger.cpp` | It forces quiet, then prints the object |
| Not this file | The flag name and the human line shape | `requirement-cpp-parameters.md` and `requirement-cpp-logger.md` |

| Includes | Excludes |
|----------|----------|
| JSON mode implies quiet, and the only product line is the JSON object | The `--json` spelling itself |

| Surface | What you open | What for |
|---------|---------------|----------|
| The terminal | stdout | One object per event |
| `src/main/cpp/logger.cpp` | Implementation | The JSON writer |

| You do… | What it means | What you type |
|---------|---------------|---------------|
| Take JSON | Quiet is on for log lines. Each verb prints JSON | `--json` or `JSON=1` |

## 2. Core rules

1. `--json` and `JSON=1` turn JSON mode on. Those names are `requirement-cpp-parameters.md`. The `Logger` constructor is the parser.
2. JSON mode implies quiet. After JSON mode is on, `logger_set_quiet` stores quiet as on. A call with `false` does not turn quiet off. `logger_get_quiet()` stays true.
3. The product line in JSON mode is one JSON object and a newline. The human line from `requirement-cpp-logger.md` is not printed before it, after it, or instead of it. `--verbose` does not add that human line.
4. Quiet suppresses every product message that is not that object. An info, warning, error, or fatal event is still emitted as the object. A debug event is emitted as the object when `requirement-cpp-debug-mode.md` says debug is on. Those events are not dropped.
5. The object is:

```json
{"timestamp":1746780000,"level":"INFO","message":"Output system initialized"}
```

| Field | Rule |
|-------|------|
| `timestamp` | `time` seconds since the epoch. Not the civil timestamp from the human line. |
| `level` | The same uppercase token as `<LEVEL>` in `requirement-cpp-logger.md`. |
| `message` | The formatted text. A quote, backslash, newline, carriage return, or tab is escaped inside the string. A byte below `0x20` is `\u00XX`. The object stays one line. |

6. The object is written with `g_print`. `help`, `about`, `list`, and `show` are not log events. In JSON mode each of them prints the object in §2.1 and does not print its human text. Without JSON mode, `help` still prints the usage text and `about` still prints the human page.

### Sample code (this project)

From `src/main/cpp/logger.cpp`. `logger_set_quiet` while JSON mode is on is the sample in `requirement-cpp-output.md`.

```cpp
static gchar *logger_json_escape(const char *text)
{
    GString *out = g_string_new("");
    const unsigned char *cursor = (const unsigned char *)(text ? text : "");

    for (; *cursor != '\0'; cursor++) {
        switch (*cursor) {
            case '\\':
                g_string_append(out, "\\\\");
                break;
            case '"':
                g_string_append(out, "\\\"");
                break;
            case '\n':
                g_string_append(out, "\\n");
                break;
            case '\r':
                g_string_append(out, "\\r");
                break;
            case '\t':
                g_string_append(out, "\\t");
                break;
            default:
                if (*cursor < 0x20) {
                    g_string_append_printf(out, "\\u%04x", *cursor);
                } else {
                    g_string_append_c(out, (gchar)*cursor);
                }
                break;
        }
    }
    return g_string_free(out, FALSE);
}
```

```cpp
        gchar *escaped = logger_json_escape(message ? message : "");
        g_print("{\"timestamp\":%ld,\"level\":\"%s\",\"message\":\"%s\"}\n",
                (long)time(NULL),
                logger_level_name(level),
                escaped);
        g_free(escaped);
```

### 2.1 Every verb

The verbs are `help`, `about`, `test`, `list`, and `show`. A run with no verb is the default run. `--json` and `JSON=1` apply to each of them. The flag may sit before or after the verb. The `Logger` constructor reads `JSON=1` and the whole argument list before it runs `help` or `about`.

| Verb | JSON result |
|------|-------------|
| default run | One object per log event, the object in rule 5. No human line |
| `test` | The same object per emitted event, including `test log`. No second envelope. The dated file still stores the human line |
| `about` | One object, then exit 0. No human page. No log line |
| `help` | One object, then exit 0. No usage text. No log line |
| `list` | One object, then the same exit as the text command. No basename lines |
| `show` | One object, then the same exit as the text command. No raw file bytes |

Quiet without JSON does not select these objects. `about` and `help` still print their text. `list` and `show` still print names and bytes.

A verb that exits 1 in JSON mode prints one object on stdout and does not print that sentence on stderr. The sentence is the `error` value. A failure that happens with no verb selected stays on stderr.

`about` uses the same reads as `requirement-cpp-about.md`. Strings stay empty when the page leaves the value empty. `inside_docker` and `tty` are booleans. `pid` is a number. `global` and `local` stay `not found` or the version string.

```json
{"command":"about","name":"cpp-logger","version":"1.1.1","description":"Console logger. Lines follow ChronicleLogger.","domain":"No database. No configuration file.","runtime_tools":"none","entry_points":"cpp-logger","host":{"stamp":"2026-11-10 10:01:34.000000","compiler":"g++ 13.3.0","cpp_standard":"C++17","operation_system":"Linux","architecture":"x86_64","current_user":"user","shell":"/bin/bash","inside_docker":false,"location":"/tmp/cpp-logger-json-smoke/cpp-logger","pid":1,"cache_folder_used":"/dev/shm/cache/cache-cpp-logger","cache_folder_preferred":"/dev/shm/cache/cache-cpp-logger","cache_folder_1st_fallback":"","cache_folder_2nd_fallback":"","persistence_storage":"","tty":false},"install":{"global_path":"/usr/local/bin/cpp-logger","global":"not found","local_path":"","local":"not found","running_from":"/tmp/cpp-logger-json-smoke/cpp-logger"}}
```

```json
{"command":"help","verbs":["help","about","test","list","show"],"flags":["--verbose","--quiet","--json","--debug","--prefix","--home","--log-datestamp"]}
```

```json
{"command":"list","folder":"/tmp/.app/cpp-logger/log","files":["cpp-logger-20261002.log","cpp-logger-20261003.log"]}
```

A missing directory is `"files":[]`, exit 0, and `folder` stays in the object.

```json
{"command":"show","file":"/tmp/.app/cpp-logger/log/cpp-logger-20261003.log","content":"..."}
```

`content` uses the same escapes as `message` in rule 5. A missing file:

```json
{"command":"show","error":"cannot read log file /tmp/.app/cpp-logger/log/cpp-logger-20261003.log"}
```

## 3. Protection rule

Future editors must not print a human log line while JSON mode is on, leave quiet off while JSON mode is on, drop an info, warning, error, or fatal event so that JSON mode prints nothing, or print the human page for `about`, `help`, `list`, or `show` while JSON mode is on.

## 4. Related artifacts

| Artifact | Role |
|----------|------|
| `docs/requirements/index.md` | Registry |
| `docs/requirements/requirement-cpp-logger.md` | Level tokens and the human line that JSON replaces |
| `docs/requirements/requirement-cpp-output.md` | Quiet storage and human-line streams |
| `docs/requirements/requirement-cpp-parameters.md` | `--json` and `JSON=1` |
| `docs/requirements/requirement-cpp-about.md` | The human page. JSON mode replaces it with the about object |
| `src/main/cpp/logger.cpp` | JSON writer and the quiet force |

**Last updated**: 2026-11-10
**Owner**: JSON
