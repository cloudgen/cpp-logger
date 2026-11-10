#!/bin/sh
# TP-CPP-OOP
# Reads logger.h, logger.cpp, and main.cpp.
# Compiles those units, the retain driver, and the format driver.
# Does not call mktemp. Does not read, assign, or delete HOME.
# Objects stay under /tmp/cpp-logger-oop-work. Cleanup deletes that directory.
set -eu
unset CONDA_PREFIX CONDA_DEFAULT_ENV VIRTUAL_ENV PYENV_VIRTUAL_ENV

cd "$(dirname "$0")/.."

fail=0

need() {
    id=$1
    file=$2
    token=$3
    if ! grep -F -q -- "$token" "$file"; then
        echo "[FAIL] ${id} missing in ${file}: ${token}"
        fail=1
    fi
}

forbid() {
    id=$1
    file=$2
    token=$3
    if grep -F -q -- "$token" "$file"; then
        echo "[FAIL] ${id} forbids in ${file}: ${token}"
        fail=1
    fi
}

header=src/main/cpp/logger.h
body=src/main/cpp/logger.cpp
entry=src/main/cpp/main.cpp

need TP-CPP-OOP "$header" 'class Logger'
need TP-CPP-OOP "$header" 'static Logger *logger_active;'
need TP-CPP-OOP "$header" 'Logger(const char *logname, const char *logdir, const char *basedir, bool is_quiet, LoggerClock clock);'
need TP-CPP-OOP "$header" 'static gint logger_filename_cmp'
need TP-CPP-OOP "$header" 'LOGGER_LEVEL_CRITICAL'
need TP-CPP-OOP "$entry" 'Logger logger(PROJECT_NAME, logdir, basedir, quiet, NULL);'
need TP-CPP-OOP "$entry" 'logger_parse_invocation(argc, argv, &logdir, &basedir, &quiet);'
need TP-CPP-OOP "$body" 'Logger::Logger(const char *logname, const char *logdir, const char *basedir, bool is_quiet, LoggerClock clock)'
forbid TP-CPP-OOP "$header" 'Logger(int argc, char **argv)'
forbid TP-CPP-OOP "$entry" 'Logger logger(argc, argv)'
forbid TP-CPP-OOP "$body" 'Logger::Logger(int argc, char **argv)'
need TP-CPP-OOP "$body" 'Logger *Logger::logger_active = NULL;'
need TP-CPP-OOP "$body" 'logger_active = this;'
need TP-CPP-OOP "$body" 'cold.logger_maintain_directory'
need TP-CPP-OOP "$body" 'Logger cold("", NULL, NULL, false, NULL);'
need TP-CPP-OOP "$body" 'va_copy(ap_copy, ap);'
forbid TP-CPP-OOP "$entry" 'logger_output_init'
forbid TP-CPP-OOP "$body" 'static bool logger_quiet_mode'
forbid TP-CPP-OOP "$body" 'static char logger_folder'

classes=$(grep -c '^class ' "$header" || true)
if [ "$classes" -ne 1 ]; then
    echo "[FAIL] TP-CPP-OOP expected one class in ${header}, found ${classes}"
    fail=1
fi

if grep -E '^[[:space:]]*(class|struct|enum|union|typedef)[[:space:]]' "$entry" >/dev/null; then
    echo "[FAIL] TP-CPP-OOP ${entry} declares a second type"
    fail=1
fi

if grep -nE 'new[[:space:]]+Logger|return[[:space:]]+Logger[[:space:]]*[;(]' "$header" "$body" "$entry" >/dev/null; then
    echo "[FAIL] TP-CPP-OOP a function returns a Logger"
    fail=1
fi

for name in \
    logger_quiet_mode logger_json_mode debug_env_enabled logger_verbose \
    logger_folder logger_log_path logger_purpose logger_prefix_arg \
    logger_home_arg logger_datestamp_arg logger_saw_quiet logger_saw_json \
    logger_saw_debug logger_saw_verbose logger_pending_page \
    logger_retention_done logger_maintain_busy \
    logger_name logger_old_log_path logger_clock
do
    need TP-CPP-OOP "$header" "$name"
done

for name in \
    logger_is_test_verb logger_set_quiet logger_get_quiet logger_get_debug \
    logger_log logger_info logger_warn logger_error logger_fatal logger_debug \
    logger_maintain_directory logger_print_help logger_print_about \
    logger_json_escape logger_command_name logger_die logger_join_base \
    logger_datestamp_ok logger_apply_base logger_set_purpose logger_list_emit \
    logger_filename_cmp logger_list_log_files logger_show_log_file \
    logger_take_value logger_log_found_switches logger_level_name \
    logger_stderr_level logger_emit_chronicle logger_unlink_regular \
    logger_retention_join logger_stamp_to_date logger_retention_stamp \
    logger_spawn_ok logger_compress_day logger_maintain_one logger_maintain_body \
    logger_append_log_file logger_about_copy logger_about_probe \
    logger_finish_command logger_critical logger_store_name
do
    need TP-CPP-OOP "$body" "Logger::${name}"
done

for name in \
    logger_log logger_is_test_verb logger_set_quiet logger_get_quiet \
    logger_get_debug logger_print_help logger_print_about \
    logger_maintain_directory
do
    need TP-CPP-OOP "$body" "logger_active->${name}"
done

# Variadic C names cannot forward "...". They pass a va_list to logger_log.
for level in LOGGER_LEVEL_INFO LOGGER_LEVEL_WARNING LOGGER_LEVEL_ERROR LOGGER_LEVEL_CRITICAL LOGGER_LEVEL_FATAL LOGGER_LEVEL_DEBUG
do
    need TP-CPP-OOP "$body" "logger_active->logger_log(${level},"
done

work=/tmp/cpp-logger-oop-work
absent=/tmp/cpp-logger-oop-absent
rm -rf "${work}" "${absent}"
mkdir -p "${work}"

g++ -std=c++17 -Wall -Wextra -Werror $(pkg-config --cflags glib-2.0) \
    -c src/main/cpp/logger.cpp -o "${work}/logger.o"
g++ -std=c++17 -Wall -Wextra -Werror $(pkg-config --cflags glib-2.0) \
    -c src/main/cpp/main.cpp -o "${work}/main.o"
g++ -std=c++17 -Wall -Wextra -Werror $(pkg-config --cflags glib-2.0) \
    -I src/main/cpp -c tests/cpp-logger-retain-driver.cpp -o "${work}/driver.o"
g++ -std=c++17 -Wall -Wextra -Werror $(pkg-config --cflags glib-2.0) \
    -I src/main/cpp -c tests/cpp-logger-format-driver.cpp -o "${work}/format.o"
g++ -std=c++17 -Wall -Wextra -Werror \
    -o "${work}/cpp-logger" "${work}/main.o" "${work}/logger.o" \
    $(pkg-config --libs glib-2.0) -pthread
g++ -std=c++17 -Wall -Wextra -Werror \
    -o "${work}/retain-driver" "${work}/driver.o" "${work}/logger.o" \
    $(pkg-config --libs glib-2.0) -pthread
g++ -std=c++17 -Wall -Wextra -Werror \
    -o "${work}/format-driver" "${work}/format.o" "${work}/logger.o" \
    $(pkg-config --libs glib-2.0) -pthread

cold_out=$("${work}/retain-driver" "${absent}" name.log 20261110 2>&1) || {
    echo "[FAIL] TP-CPP-OOP unbound maintain exited non-zero"
    fail=1
}
if [ -n "${cold_out}" ]; then
    echo "[FAIL] TP-CPP-OOP unbound maintain printed: ${cold_out}"
    fail=1
fi
if [ -e "${absent}" ]; then
    echo "[FAIL] TP-CPP-OOP unbound maintain created a directory"
    fail=1
fi

empty_dir=/tmp/cpp-logger-format-empty
lines_dir=/tmp/cpp-logger-format-log
quiet_dir=/tmp/cpp-logger-format-quiet
fail_dir=/tmp/cpp-logger-format-fail
rm -rf "${empty_dir}" "${lines_dir}" "${quiet_dir}" "${fail_dir}"

if ! empty_out=$("${work}/format-driver" empty 2>"${work}/empty.err"); then
    echo "[FAIL] TP-CPP-OOP empty name exited non-zero"
    fail=1
fi
if [ -n "${empty_out}" ] || [ -s "${work}/empty.err" ] || [ -e "${empty_dir}" ]; then
    echo "[FAIL] TP-CPP-OOP empty name created a directory or printed"
    fail=1
fi

if ! lines_out=$("${work}/format-driver" lines 2>"${work}/lines.err"); then
    echo "[FAIL] TP-CPP-OOP format lines exited non-zero"
    fail=1
fi
if ! printf '%s\n' "${lines_out}" | grep -F 'Created directory: /tmp/cpp-logger-format-log' >/dev/null; then
    echo "[FAIL] TP-CPP-OOP format lines missed Created directory"
    fail=1
fi
if ! grep -F '[CRITICAL] @database :] Connected' "${work}/lines.err" >/dev/null; then
    echo "[FAIL] TP-CPP-OOP CRITICAL was not on stderr"
    fail=1
fi
stamp=$(date +%Y%m%d)
line_file="${lines_dir}/cpp-logger-${stamp}.log"
if [ ! -f "${line_file}" ]; then
    echo "[FAIL] TP-CPP-OOP kebab log file missing: ${line_file}"
    fail=1
else
    first=$(od -An -t x1 -N 1 "${line_file}" | tr -d ' \n')
    if [ "${first}" != "0a" ]; then
        echo "[FAIL] TP-CPP-OOP construction did not start with a newline"
        fail=1
    fi
    if ! grep -F '[INFO] :] no component' "${line_file}" >/dev/null; then
        echo "[FAIL] TP-CPP-OOP empty component line missing"
        fail=1
    fi
    if ! grep -F '[CRITICAL] @database :] Connected' "${line_file}" >/dev/null; then
        echo "[FAIL] TP-CPP-OOP CRITICAL line missing from the file"
        fail=1
    fi
fi
if [ -e "${lines_dir}/CppLogger-${stamp}.log" ]; then
    echo "[FAIL] TP-CPP-OOP kept an internal capital in the file name"
    fail=1
fi

if ! quiet_out=$("${work}/format-driver" quiet 2>"${work}/quiet.err"); then
    echo "[FAIL] TP-CPP-OOP quiet construction exited non-zero"
    fail=1
fi
if printf '%s\n' "${quiet_out}" | grep -F 'Created directory:' >/dev/null; then
    echo "[FAIL] TP-CPP-OOP quiet printed Created directory"
    fail=1
fi
if [ ! -d "${quiet_dir}" ]; then
    echo "[FAIL] TP-CPP-OOP quiet did not create the log directory"
    fail=1
fi

if ! "${work}/format-driver" fail >/dev/null 2>"${work}/fail.err"; then
    echo "[FAIL] TP-CPP-OOP clock failure exited non-zero"
    fail=1
fi
fail_file="${fail_dir}/cpp-logger-19700101.log"
if [ ! -f "${fail_file}" ]; then
    echo "[FAIL] TP-CPP-OOP clock failure did not use 19700101"
    fail=1
elif ! grep -F '[1970-01-01 00:00:00] pid:' "${fail_file}" >/dev/null || ! grep -F '[INFO] @main :] clock failed' "${fail_file}" >/dev/null; then
    echo "[FAIL] TP-CPP-OOP clock failure line missing"
    fail=1
fi

rm -rf "${work}" "${absent}" "${empty_dir}" "${lines_dir}" "${quiet_dir}" "${fail_dir}"

if [ "$fail" -ne 0 ]; then
    exit 1
fi

echo "PASS cpp oop"
