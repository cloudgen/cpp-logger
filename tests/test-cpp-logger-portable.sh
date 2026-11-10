#!/bin/sh
# Warning: do not call mktemp or mkstemp.
# Do not read, expand, assign, or delete the home environment variable.
# The child is started with HOME and PREFIX removed.
# Cleanup removes /tmp/.app/<name>, /tmp/.app when that directory is empty,
# and /tmp/cpp-logger-portable-work. It does not remove /tmp.
set -eu
unset CONDA_PREFIX CONDA_DEFAULT_ENV VIRTUAL_ENV PYENV_VIRTUAL_ENV

cd "$(dirname "$0")/.."

prj=$(sed -n 's/^PRJ=//p' project.ini | head -n 1)
version=$(sed -n 's/^VERSION=//p' project.ini | head -n 1)
name=$(sed -n 's/^#define PROJECT_NAME[[:space:]]*"\(.*\)".*/\1/p' src/main/cpp/project.h | head -n 1)
major=$(sed -n 's/^#define VERSION_MAJOR[[:space:]]*\([0-9][0-9]*\).*/\1/p' src/main/cpp/project.h | head -n 1)
minor=$(sed -n 's/^#define VERSION_MINOR[[:space:]]*\([0-9][0-9]*\).*/\1/p' src/main/cpp/project.h | head -n 1)
patch=$(sed -n 's/^#define VERSION_PATCH[[:space:]]*\([0-9][0-9]*\).*/\1/p' src/main/cpp/project.h | head -n 1)
component=$(sed -n 's/^#define LOG_COMPONENT[[:space:]]*"\(.*\)".*/\1/p' src/main/cpp/project.h | head -n 1)
triple="${major}.${minor}.${patch}"

if [ -z "${prj}" ] || [ -z "${name}" ] || [ "${prj}" != "${name}" ]; then
    echo "[ERROR] PRJ and PROJECT_NAME are not one name"
    exit 1
fi
if [ -z "${version}" ] || [ "${version}" != "${triple}" ]; then
    echo "[ERROR] VERSION and the version triple are not one version"
    exit 1
fi
if [ "${component}" != "main" ]; then
    echo "[ERROR] LOG_COMPONENT is not main"
    exit 1
fi

if grep -F "/var/${name}" src/main/cpp/logger.cpp >/dev/null; then
    echo "[ERROR] a log path hard-codes /var/${name}"
    exit 1
fi
if grep -F ".app/${name}" src/main/cpp/logger.cpp >/dev/null; then
    echo "[ERROR] a log path hard-codes .app/${name}"
    exit 1
fi
if grep -F "cache-${name}" src/main/cpp/logger.cpp >/dev/null; then
    echo "[ERROR] a cache path hard-codes cache-${name}"
    exit 1
fi
if grep -F "/var/cache/${name}" src/main/cpp/logger.cpp >/dev/null; then
    echo "[ERROR] a cache path hard-codes /var/cache/${name}"
    exit 1
fi

work=/tmp/cpp-logger-portable-work
home_log="/tmp/.app/${name}/log/${name}-20261003.log"
rm -rf "${work}" "/tmp/.app/${name}"
rmdir /tmp/.app 2>/dev/null || true
mkdir "${work}"

g++ -std=c++17 -Wall -Wextra -Werror $(pkg-config --cflags glib-2.0) \
    -c src/main/cpp/logger.cpp -o "${work}/logger.o"
g++ -std=c++17 -Wall -Wextra -Werror $(pkg-config --cflags glib-2.0) \
    -c src/main/cpp/main.cpp -o "${work}/main.o"
g++ -std=c++17 -Wall -Wextra -Werror \
    -o "${work}/cpp-logger" "${work}/main.o" "${work}/logger.o" \
    $(pkg-config --libs glib-2.0) -pthread

bin="${work}/cpp-logger"
help_out=$(env -u HOME -u PREFIX "${bin}" help)
printf '%s\n' "${help_out}" | grep -F "Usage: ${name}" >/dev/null
printf '%s\n' "${help_out}" | grep -F "/var/${name}/log" >/dev/null
printf '%s\n' "${help_out}" | grep -F ".app/${name}/log" >/dev/null
printf '%s\n' "${help_out}" | grep -F "${name}-<YYYYMMDD>.log" >/dev/null

env -u HOME -u PREFIX "${bin}" test --home /tmp --log-datestamp 20261003 >"${work}/test.txt" 2>&1
if [ ! -f "${home_log}" ]; then
    echo "[ERROR] the dated file is not under .app/${name}/log"
    exit 1
fi
grep -F '] @main :] test log' "${home_log}" >/dev/null
if [ -e "/tmp/var/${name}" ]; then
    echo "[ERROR] --home created the root log directory"
    exit 1
fi

rm -rf "${work}" "/tmp/.app/${name}"
rmdir /tmp/.app 2>/dev/null || true
echo "PASS cpp logger portable"
