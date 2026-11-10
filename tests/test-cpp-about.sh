#!/bin/sh
# Warning: do not call mktemp or mkstemp.
# Do not read, expand, assign, or delete the home environment variable.
# This check builds one binary under /tmp. The child is started with HOME removed.
# Cleanup deletes /tmp/cpp-logger-about-work. It does not delete /tmp.
set -eu

cd "$(dirname "$0")/.."

work=/tmp/cpp-logger-about-work
bin="${work}/cpp-logger"
preferred=/dev/shm/cache/cache-cpp-logger

rm -rf "${work}"
rm -rf /tmp/.app/cpp-logger
rmdir /tmp/.app 2>/dev/null || true
mkdir "${work}"

had_preferred=0
if [ -e "${preferred}" ]; then
    had_preferred=1
fi

g++ -std=c++17 -Wall -Wextra -Werror $(pkg-config --cflags glib-2.0) \
    -c src/main/cpp/logger.cpp -o "${work}/logger.o"
g++ -std=c++17 -Wall -Wextra -Werror $(pkg-config --cflags glib-2.0) \
    -c src/main/cpp/main.cpp -o "${work}/main.o"
g++ -std=c++17 -Wall -Wextra -Werror \
    -o "${bin}" "${work}/main.o" "${work}/logger.o" \
    $(pkg-config --libs glib-2.0) -pthread

if [ "$(id -u)" -eq 0 ]; then
    echo "[ERROR] TP-CPP-ABOUT runs as a normal user"
    exit 1
fi

if ! about_out=$(env -u HOME -u PREFIX USER=qa-about-user SHELL=/bin/sh "${bin}" about); then
    echo "[ERROR] about exited non-zero"
    exit 1
fi

need_line() {
    if ! printf '%s\n' "${about_out}" | grep -F -x "$1" >/dev/null; then
        echo "[ERROR] missing line: $1"
        exit 1
    fi
}

need_line "cpp-logger 1.1.1"
need_line "Console logger. Lines follow ChronicleLogger."
need_line "Domain: No database. No configuration file."
need_line "Runtime tools: none"
need_line "Entry points: cpp-logger"
need_line "  Now checking your operation system!"

# The compiler line names the literal g++. The page does not spawn the compiler.
compiler_line=$(printf '%s\n' "${about_out}" | grep -F '    Compiler: ')
case "${compiler_line}" in
    "    Compiler: g++ "*) ;;
    *)
        echo "[ERROR] compiler line: ${compiler_line}"
        exit 1
        ;;
esac

need_line "    C++ standard: C++17"
need_line "    Current User: qa-about-user"
need_line "    Shell: /bin/sh"
need_line "    Cache folder used: ${preferred}"
need_line "    Cache folder (preferred): ${preferred}"
need_line "    Cache folder (1st fallback): "
need_line "    Cache folder (2nd fallback): "
need_line "    Persistence storage: "
need_line "    TTY / Interactive: no"

os_name=$(uname -s)
os_rel=$(uname -r)
need_line "    Operation System: ${os_name} ${os_rel}"
need_line "    Architecture: $(uname -m)"

running_from=$(readlink -f "${bin}")
case "${about_out}" in
    *"$(printf '%s' "${running_from}")"*) ;;
    *)
        echo "[ERROR] location is not the running binary"
        exit 1
        ;;
esac

global_bin=/usr/local/bin/cpp-logger
if [ -f "${global_bin}" ] && [ -x "${global_bin}" ]; then
    global_value=1.1.1
else
    global_value="not found"
fi
need_line "Install location"
need_line "    Global install (${global_bin}): ${global_value}"
need_line "    Local install: not found"
need_line "    Running from: ${running_from}"

if ! printf '%s\n' "${about_out}" | grep -E '^[0-9]{4}-[0-9]{2}-[0-9]{2} [0-9]{2}:[0-9]{2}:[0-9]{2}\.000000 cpp-logger\(v1\.1\.1\)  \[CHECK SYSTEM\]:$' >/dev/null; then
    echo "[ERROR] host-check header"
    exit 1
fi

if ! printf '%s\n' "${about_out}" | grep -E '^    Inside docker container: (True|False)$' >/dev/null; then
    echo "[ERROR] docker line"
    exit 1
fi

if ! printf '%s\n' "${about_out}" | grep -E '^    PID: [0-9]+$' >/dev/null; then
    echo "[ERROR] pid line"
    exit 1
fi

# Identity, a blank line, the host check, a blank line, then install location.
printf '%s\n' "${about_out}" | awk '
    BEGIN { blank = 0; header = 0; n = 0; sep = 0; install = 0; g = 0; l = 0; r = 0
        want[0] = "Compiler"
        want[1] = "C++ standard"
        want[2] = "Operation System"
        want[3] = "Architecture"
        want[4] = "Current User"
        want[5] = "Shell"
        want[6] = "Inside docker container"
        want[7] = "Location"
        want[8] = "PID"
        want[9] = "Cache folder used"
        want[10] = "Cache folder (preferred)"
        want[11] = "Cache folder (1st fallback)"
        want[12] = "Cache folder (2nd fallback)"
        want[13] = "Persistence storage"
        want[14] = "TTY / Interactive"
    }
    NR == 6 && $0 != "" { print "[ERROR] blank line missing after identity"; exit 1 }
    NR == 6 { blank = 1; next }
    NR == 7 && $0 !~ /  \[CHECK SYSTEM\]:$/ { print "[ERROR] host check does not follow the blank line"; exit 1 }
    NR == 7 { header = 1; next }
    header && sep == 0 && $0 ~ /^    / {
        label = substr($0, 5)
        sub(/:.*$/, "", label)
        if (label != want[n]) {
            print "[ERROR] label order at " n ": " label
            exit 1
        }
        n++
        next
    }
    header && sep == 0 && n == 15 && $0 == "" { sep = 1; next }
    header && sep == 1 && install == 0 && $0 == "Install location" { install = 1; next }
    header && install == 1 && g == 0 && $0 ~ /^    Global install / { g = 1; next }
    header && g == 1 && l == 0 && $0 ~ /^    Local install/ { l = 1; next }
    header && l == 1 && r == 0 && $0 ~ /^    Running from: / { r = 1; next }
    header && r == 1 && $0 != "" { print "[ERROR] trailing line: " $0; exit 1 }
    END {
        if (!blank || !header || n != 15 || !sep || !install || !g || !l || !r) {
            print "[ERROR] page blocks are incomplete"
            exit 1
        }
    }
'

for banned in Python conda pyenv CheckSystem "C Library" "curl -fsSL" "GLOBAL INSTALLED" "LOCAL INSTALLED" "UNINSTALLED" "properly installed" "Repository:" "Install channel:"; do
    if printf '%s\n' "${about_out}" | grep -F "${banned}" >/dev/null; then
        echo "[ERROR] page contains ${banned}"
        exit 1
    fi
done

page_still() {
    mode_out=$(env -u HOME -u PREFIX "$@") || {
        echo "[ERROR] about mode exited non-zero"
        exit 1
    }
    if ! printf '%s\n' "${mode_out}" | grep -F '  [CHECK SYSTEM]:' >/dev/null; then
        echo "[ERROR] host check hidden"
        exit 1
    fi
    if ! printf '%s\n' "${mode_out}" | grep -F -x 'Install location' >/dev/null; then
        echo "[ERROR] install location hidden"
        exit 1
    fi
    if ! printf '%s\n' "${mode_out}" | grep -F '    Running from: ' >/dev/null; then
        echo "[ERROR] running path hidden"
        exit 1
    fi
    first=$(printf '%s\n' "${mode_out}" | head -n 1)
    case "${first}" in
        "{"*)
            echo "[ERROR] about printed JSON"
            exit 1
            ;;
    esac
}

page_still "${bin}" --quiet about

page_json() {
    mode_out=$(env -u HOME -u PREFIX "$@") || {
        echo "[ERROR] about JSON exited non-zero"
        exit 1
    }
    if [ "$(printf '%s\n' "${mode_out}" | wc -l)" -ne 1 ]; then
        echo "[ERROR] about JSON is not one line"
        exit 1
    fi
    printf '%s\n' "${mode_out}" | grep -F '"command":"about"' >/dev/null
    printf '%s\n' "${mode_out}" | grep -F '"name":"cpp-logger"' >/dev/null
    printf '%s\n' "${mode_out}" | grep -F '"version":"1.1.1"' >/dev/null
    printf '%s\n' "${mode_out}" | grep -F '"cpp_standard":"C++17"' >/dev/null
    printf '%s\n' "${mode_out}" | grep -F '"runtime_tools":"none"' >/dev/null
    printf '%s\n' "${mode_out}" | grep -F '"domain":"No database. No configuration file."' >/dev/null
    printf '%s\n' "${mode_out}" | grep -F '"cache_folder_preferred":"/dev/shm/cache/cache-cpp-logger"' >/dev/null
    printf '%s\n' "${mode_out}" | grep -F '"cache_folder_1st_fallback":""' >/dev/null
    printf '%s\n' "${mode_out}" | grep -F '"cache_folder_2nd_fallback":""' >/dev/null
    printf '%s\n' "${mode_out}" | grep -F '"persistence_storage":""' >/dev/null
    printf '%s\n' "${mode_out}" | grep -F '"local":"not found"' >/dev/null
    printf '%s\n' "${mode_out}" | grep -F '"global_path":"/usr/local/bin/cpp-logger"' >/dev/null
    if printf '%s\n' "${mode_out}" | grep -F '[CHECK SYSTEM]' >/dev/null; then
        echo "[ERROR] about JSON included the human page"
        exit 1
    fi
    if printf '%s\n' "${mode_out}" | grep -F 'Install location' >/dev/null; then
        echo "[ERROR] about JSON included the human install block"
        exit 1
    fi
}

page_json "${bin}" --json about
page_json "${bin}" about --json
page_json env JSON=1 "${bin}" about

if [ -d /tmp/.app/cpp-logger ]; then
    echo "[ERROR] about created a log directory"
    exit 1
fi
if [ "${had_preferred}" -eq 0 ] && [ -e "${preferred}" ]; then
    echo "[ERROR] about created the cache directory"
    exit 1
fi

rm -rf "${work}"
echo "PASS cpp about"
