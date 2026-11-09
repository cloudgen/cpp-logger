#!/bin/sh
# Warning: do not call mktemp or mkstemp.
# Do not read, expand, assign, or delete the home environment variable.
# If a new home assignment fails, the shell keeps the original home.
# Removing that directory would destroy the user account.
# This check builds one binary and runs the test command with the literal directory /tmp.
# --home /tmp writes /tmp/.app/cpp-logger/log/cpp-logger-20261003.log.
# --prefix /tmp writes /tmp/var/cpp-logger/log/cpp-logger-20261003.log.
# Cleanup removes those two trees, empty parent directories, and the work directory.
# It does not remove /tmp and it does not start a normal run.
set -eu

cd "$(dirname "$0")/.."

work=/tmp/cpp-logger-folder-work
home_log=/tmp/.app/cpp-logger/log/cpp-logger-20261003.log
home_folder=/tmp/.app/cpp-logger/log
prefix_log=/tmp/var/cpp-logger/log/cpp-logger-20261003.log
prefix_folder=/tmp/var/cpp-logger/log
home_decoy=/tmp/cpp-logger-home-decoy
prefix_decoy=/tmp/cpp-logger-prefix-decoy

rm -rf "${work}" "${home_decoy}" "${prefix_decoy}"
rm -rf /tmp/.app/cpp-logger
rm -rf /tmp/var/cpp-logger
rmdir /tmp/.app 2>/dev/null || true
rmdir /tmp/var 2>/dev/null || true
mkdir "${work}"

g++ -std=c++17 -Wall -Wextra -Werror $(pkg-config --cflags glib-2.0) \
    -c src/main/cpp/logger.cpp -o "${work}/logger.o"
g++ -std=c++17 -Wall -Wextra -Werror $(pkg-config --cflags glib-2.0) \
    -c src/main/cpp/main.cpp -o "${work}/main.o"
g++ -std=c++17 -Wall -Wextra -Werror \
    -o "${work}/cpp-logger" "${work}/main.o" "${work}/logger.o" \
    $(pkg-config --libs glib-2.0) -pthread

bin="${work}/cpp-logger"

if env -u HOME -u PREFIX "${bin}" test --debug >"${work}/no-root.txt" 2>&1; then
    echo "[ERROR] test command ran without a directory switch"
    exit 1
fi
grep -F 'test purpose command requires --prefix or --home' "${work}/no-root.txt" >/dev/null
if [ -e "${home_log}" ] || [ -e "${prefix_log}" ]; then
    echo "[ERROR] a log file appeared without a directory switch"
    exit 1
fi

if env -u HOME -u PREFIX "${bin}" test --home /tmp >"${work}/miss-stamp.txt" 2>&1; then
    echo "[ERROR] test command ran without a datestamp"
    exit 1
fi
grep -F 'test purpose command requires --log-datestamp YYYYMMDD' "${work}/miss-stamp.txt" >/dev/null
if [ -e "${home_log}" ] || [ -e "${prefix_log}" ]; then
    echo "[ERROR] a log file appeared without a datestamp"
    exit 1
fi

if env -u HOME -u PREFIX "${bin}" test --home /tmp --log-datestamp 2026 >"${work}/bad-stamp.txt" 2>&1; then
    echo "[ERROR] test command accepted a short datestamp"
    exit 1
fi
grep -F -- '--log-datestamp requires YYYYMMDD' "${work}/bad-stamp.txt" >/dev/null
if grep -F 'test purpose command requires --log-datestamp' "${work}/bad-stamp.txt" >/dev/null; then
    echo "[ERROR] a present stamp was reported as missing"
    exit 1
fi
if [ -e "${home_log}" ] || [ -e "${prefix_log}" ]; then
    echo "[ERROR] a rejected datestamp created a log file"
    exit 1
fi

debug_out=$(env -u HOME -u PREFIX "${bin}" test --home /tmp --log-datestamp 20261003 --debug)
printf '%s\n' "${debug_out}" | grep -F '] @main :] test log' >/dev/null
printf '%s\n' "${debug_out}" | grep -F '] @main :] switch --debug' >/dev/null
printf '%s\n' "${debug_out}" | grep -F '] @main :] component main start' >/dev/null
printf '%s\n' "${debug_out}" | grep -F '] @main :] component main end' >/dev/null
if printf '%s\n' "${debug_out}" | grep -F 'Starting cpp-logger' >/dev/null; then
    echo "[ERROR] test purpose command printed the normal-run start line"
    exit 1
fi
grep -F '] @main :] test log' "${home_log}" >/dev/null
grep -F '] @main :] switch --debug' "${home_log}" >/dev/null
grep -F '] @main :] component main start' "${home_log}" >/dev/null
grep -F '] @main :] component main end' "${home_log}" >/dev/null

home_out=$(env -u HOME -u PREFIX "${bin}" test --home /tmp --log-datestamp 20261003 --debug --verbose)
printf '%s\n' "${home_out}" | grep -F "logger folder ${home_folder}" >/dev/null
printf '%s\n' "${home_out}" | grep -F "log file ${home_log}" >/dev/null
if [ ! -f "${home_log}" ]; then
    echo "[ERROR] other-user rehearsal file missing"
    exit 1
fi
grep -F '] @main :] test log' "${home_log}" >/dev/null
if [ -d /tmp/var/cpp-logger ]; then
    echo "[ERROR] --home created the root-or-sudo rehearsal directory"
    exit 1
fi

quiet_out=$(env -u HOME -u PREFIX "${bin}" test --home /tmp --log-datestamp 20261003 --verbose --quiet)
if printf '%s\n' "${quiet_out}" | grep -F '] @main :]' >/dev/null; then
    echo "[ERROR] --verbose showed a stdout log line while quiet"
    exit 1
fi

json_out=$(env -u HOME -u PREFIX "${bin}" test --home /tmp --log-datestamp 20261003 --verbose --json)
printf '%s\n' "${json_out}" | grep -F '"level":"INFO"' >/dev/null
if printf '%s\n' "${json_out}" | grep -F '] @main :]' >/dev/null; then
    echo "[ERROR] --verbose added a human line in JSON mode"
    exit 1
fi

prefix_out=$(env -u HOME -u PREFIX "${bin}" test --prefix /tmp --log-datestamp 20261003 --debug --verbose)
printf '%s\n' "${prefix_out}" | grep -F "logger folder ${prefix_folder}" >/dev/null
printf '%s\n' "${prefix_out}" | grep -F "log file ${prefix_log}" >/dev/null
if [ ! -f "${prefix_log}" ]; then
    echo "[ERROR] root-or-sudo rehearsal file missing"
    exit 1
fi
grep -F '] @main :] test log' "${prefix_log}" >/dev/null

home_before=$(grep -F '] @main :] test log' "${home_log}" | wc -l | tr -d ' ')
env -u HOME -u PREFIX "${bin}" test --prefix /tmp --home "${home_decoy}" --log-datestamp 20261003 >/dev/null
if [ -d "${home_decoy}" ]; then
    echo "[ERROR] --home won over --prefix"
    exit 1
fi
home_after=$(grep -F '] @main :] test log' "${home_log}" | wc -l | tr -d ' ')
if [ "${home_before}" != "${home_after}" ]; then
    echo "[ERROR] --prefix wrote the other-user rehearsal file"
    exit 1
fi
prefix_count=$(grep -F '] @main :] test log' "${prefix_log}" | wc -l | tr -d ' ')
if [ "${prefix_count}" -lt 2 ]; then
    echo "[ERROR] --prefix did not append its own file"
    exit 1
fi

env -u HOME PREFIX="${prefix_decoy}" "${bin}" test --home /tmp --log-datestamp 20261003 >/dev/null
if [ -d "${prefix_decoy}" ]; then
    echo "[ERROR] test command used PREFIX"
    exit 1
fi

help_out=$(env -u HOME -u PREFIX "${bin}" help)
printf '%s\n' "${help_out}" | grep -F -- '--verbose' >/dev/null
printf '%s\n' "${help_out}" | grep -F -- '--log-datestamp' >/dev/null
printf '%s\n' "${help_out}" | grep -F '/var/' >/dev/null
printf '%s\n' "${help_out}" | grep -F '/.app/' >/dev/null

echo "----- ${home_log} -----"
cat "${home_log}"
echo "----- ${prefix_log} -----"
cat "${prefix_log}"
echo "PASS logger folder"

rm -rf /tmp/.app/cpp-logger
rm -rf /tmp/var/cpp-logger
rmdir /tmp/.app 2>/dev/null || true
rmdir /tmp/var 2>/dev/null || true
rm -rf "${work}" "${home_decoy}" "${prefix_decoy}"
