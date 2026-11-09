**file**: docs/requirements/requirement-cpp-json.md
**Status**: Active
**Updated**: 2026-10-03
**Philosophy**: CIAO v2.10.2 / CIAO-Lite (Caution • Intentional • Anti-fragile • Over-engineered / Over-protect)

## 1. Purpose

This file owns JSON mode. JSON mode implies quiet so the terminal shows the JSON object and does not show any other product message.

### 1.1 Human-facing

**In one sentence:** You pass `--json` and each log event is one JSON object, with no ChronicleLogger line beside it.

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
| Take a JSON log | Quiet is on. Human lines stay off. Each allowed event is one object | `--json` or `JSON=1` |

## 2. Core rules

1. `--json` and `JSON=1` turn JSON mode on. Those names are `requirement-cpp-parameters.md`. `logger_output_init` is the parser.
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

6. The object is written with `g_print`. `help` is not a log event. `help` still prints the usage text and exits.

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

## 3. Protection rule

Future editors must not print a human log line while JSON mode is on, leave quiet off while JSON mode is on, or drop an info, warning, error, or fatal event so that JSON mode prints nothing.

## 4. Related artifacts

| Artifact | Role |
|----------|------|
| `docs/requirements/index.md` | Registry |
| `docs/requirements/requirement-cpp-logger.md` | Level tokens and the human line that JSON replaces |
| `docs/requirements/requirement-cpp-output.md` | Quiet storage and human-line streams |
| `docs/requirements/requirement-cpp-parameters.md` | `--json` and `JSON=1` |
| `src/main/cpp/logger.cpp` | JSON writer and the quiet force |

**Last updated**: 2026-10-03
**Owner**: JSON
