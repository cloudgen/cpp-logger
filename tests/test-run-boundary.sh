#!/bin/sh
# Warning: do not call mktemp or mkstemp.
# Do not read, expand, assign, or delete the home environment variable.
# This check builds one binary under /tmp and uses the literal directory /tmp.
# A normal run is started with HOME and PREFIX removed, so it does not write a login log file.
set -eu
unset CONDA_PREFIX CONDA_DEFAULT_ENV VIRTUAL_ENV PYENV_VIRTUAL_ENV

cd "$(dirname "$0")/.."

work=/tmp/cpp-logger-boundary-work
home_dir=/tmp/.app/cpp-logger/log
home_log=${home_dir}/cpp-logger-20261003.log

rm -rf "${work}"
rm -rf /tmp/.app/cpp-logger
rm -rf /tmp/var/cpp-logger
rmdir /tmp/.app 2>/dev/null || true
rmdir /tmp/var 2>/dev/null || true
mkdir "${work}"

g++ -std=c++17 -Wall -Wextra -Werror $(pkg-config --cflags glib-2.0) \
    -c src/main/cpp/logger.cpp -o "${work}/logger.o"
g++ -std=c++17 -Wall -Wextra -Werror $(pkg-config --cflags glib-2.0) \
    -c src/main/cpp/about.cpp -o "${work}/about.o"
g++ -std=c++17 -Wall -Wextra -Werror $(pkg-config --cflags glib-2.0) \
    -c src/main/cpp/aboutcli.cpp -o "${work}/aboutcli.o"
g++ -std=c++17 -Wall -Wextra -Werror $(pkg-config --cflags glib-2.0) \
    -c src/main/cpp/help.cpp -o "${work}/help.o"
g++ -std=c++17 -Wall -Wextra -Werror $(pkg-config --cflags glib-2.0) \
    -c src/main/cpp/invocation.cpp -o "${work}/invocation.o"
g++ -std=c++17 -Wall -Wextra -Werror $(pkg-config --cflags glib-2.0) \
    -c src/main/cpp/main.cpp -o "${work}/main.o"
g++ -std=c++17 -Wall -Wextra -Werror \
    -o "${work}/cpp-logger" "${work}/main.o" "${work}/logger.o" "${work}/about.o" "${work}/aboutcli.o" "${work}/help.o" "${work}/invocation.o" \
    $(pkg-config --libs glib-2.0) -pthread

bin="${work}/cpp-logger"

about_out=$(env -u HOME -u PREFIX "${bin}" about)
printf '%s\n' "${about_out}" | grep -F 'cpp-logger 1.2.1' >/dev/null
printf '%s\n' "${about_out}" | grep -F 'No database. No configuration file.' >/dev/null
if [ -d /tmp/.app/cpp-logger ]; then
    echo "[ERROR] about created a log directory"
    exit 1
fi

help_out=$(env -u HOME -u PREFIX "${bin}" help)
printf '%s\n' "${help_out}" | grep -F 'about' >/dev/null
if [ -d /tmp/.app/cpp-logger ]; then
    echo "[ERROR] help created a log directory"
    exit 1
fi
help_json=$(env -u HOME -u PREFIX "${bin}" help --json)
printf '%s\n' "${help_json}" | grep -F '"command":"help"' >/dev/null
printf '%s\n' "${help_json}" | grep -F '"about"' >/dev/null
if printf '%s\n' "${help_json}" | grep -F 'Usage:' >/dev/null; then
    echo "[ERROR] help --json printed the usage text"
    exit 1
fi
help_env=$(env -u HOME -u PREFIX JSON=1 "${bin}" help)
if [ "${help_env}" != "${help_json}" ]; then
    echo "[ERROR] JSON=1 help differed from help --json"
    exit 1
fi

plain_out=$(env -u HOME -u PREFIX "${bin}")
if printf '%s\n' "${plain_out}" | grep -F '] @main :]' >/dev/null; then
    echo "[ERROR] a normal run showed a human log line without --verbose or --debug"
    exit 1
fi

verbose_out=$(env -u HOME -u PREFIX "${bin}" --verbose)
printf '%s\n' "${verbose_out}" | grep -F '] @main :] Starting cpp-logger 1.2.1' >/dev/null
printf '%s\n' "${verbose_out}" | grep -F '] @main :] Finished cpp-logger 1.2.1' >/dev/null
if printf '%s\n' "${verbose_out}" | grep -F 'component main start' >/dev/null; then
    echo "[ERROR] component start was logged without debug mode"
    exit 1
fi

debug_out=$(env -u HOME -u PREFIX "${bin}" --debug)
printf '%s\n' "${debug_out}" | grep -F '] @main :] switch --debug' >/dev/null
printf '%s\n' "${debug_out}" | grep -F '] @main :] component main start' >/dev/null
printf '%s\n' "${debug_out}" | grep -F '] @main :] component main end' >/dev/null
printf '%s\n' "${debug_out}" | grep -F '] @main :] Starting cpp-logger 1.2.1' >/dev/null
printf '%s\n' "${debug_out}" | grep -F '] @main :] Finished cpp-logger 1.2.1' >/dev/null
if printf '%s\n' "${debug_out}" | grep -F 'switch --quiet' >/dev/null; then
    echo "[ERROR] a switch that was not passed was logged"
    exit 1
fi

quiet_out=$(env -u HOME -u PREFIX "${bin}" --debug --quiet)
if printf '%s\n' "${quiet_out}" | grep -F '] @main :]' >/dev/null; then
    echo "[ERROR] --debug showed a human log line while quiet"
    exit 1
fi

yes_out=$(env -u HOME -u PREFIX DEBUG=yes "${bin}")
if printf '%s\n' "${yes_out}" | grep -F '] @main :]' >/dev/null; then
    echo "[ERROR] DEBUG=yes enabled debug"
    exit 1
fi

one_out=$(env -u HOME -u PREFIX DEBUG=1 "${bin}")
printf '%s\n' "${one_out}" | grep -F '] @main :] component main start' >/dev/null
printf '%s\n' "${one_out}" | grep -F '] @main :] Starting cpp-logger 1.2.1' >/dev/null
if printf '%s\n' "${one_out}" | grep -F 'switch --debug' >/dev/null; then
    echo "[ERROR] DEBUG=1 logged a switch that was not passed"
    exit 1
fi

show_out=$(env -u HOME -u PREFIX DEBUG=show "${bin}")
printf '%s\n' "${show_out}" | grep -F '] @main :] component main start' >/dev/null

true_out=$(env -u HOME -u PREFIX DEBUG=TRUE "${bin}")
printf '%s\n' "${true_out}" | grep -F '] @main :] component main start' >/dev/null

low_out=$(env -u HOME -u PREFIX debug=1 "${bin}")
printf '%s\n' "${low_out}" | grep -F '] @main :] component main start' >/dev/null

json_out=$(env -u HOME -u PREFIX "${bin}" --debug --json)
printf '%s\n' "${json_out}" | grep -F '"message":"switch --debug"' >/dev/null
printf '%s\n' "${json_out}" | grep -F '"message":"component main start"' >/dev/null
if printf '%s\n' "${json_out}" | grep -F '] @main :]' >/dev/null; then
    echo "[ERROR] --debug added a human line in JSON mode"
    exit 1
fi

env -u HOME -u PREFIX "${bin}" test --home /tmp --log-datestamp 20261003 --debug --quiet >/dev/null
grep -F '] @main :] switch --debug' "${home_log}" >/dev/null
grep -F '] @main :] switch --quiet' "${home_log}" >/dev/null
grep -F '] @main :] switch --home /tmp' "${home_log}" >/dev/null
grep -F '] @main :] switch --log-datestamp 20261003' "${home_log}" >/dev/null
grep -F '] @main :] component main start' "${home_log}" >/dev/null
grep -F '] @main :] component main end' "${home_log}" >/dev/null
grep -F '] @main :] test log' "${home_log}" >/dev/null
if grep -F 'Starting cpp-logger' "${home_log}" >/dev/null; then
    echo "[ERROR] test purpose command stored the normal-run start line"
    exit 1
fi

rm -rf "${work}"
rm -rf /tmp/.app/cpp-logger
rm -rf /tmp/var/cpp-logger
rmdir /tmp/.app 2>/dev/null || true
rmdir /tmp/var 2>/dev/null || true

echo "PASS run boundary"
