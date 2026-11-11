#!/bin/sh
# Warning: do not call mktemp or mkstemp.
# Do not read, expand, assign, or delete the home environment variable.
# Scratch stays under literal /tmp directories.
# The child is started with HOME and PREFIX removed.
# Cleanup removes those scratch directories. It does not remove /tmp.
set -eu
unset CONDA_PREFIX CONDA_DEFAULT_ENV VIRTUAL_ENV PYENV_VIRTUAL_ENV

cd "$(dirname "$0")/.."

name=$(sed -n 's/^#define PROJECT_NAME[[:space:]]*"\(.*\)".*/\1/p' src/main/cpp/project.h | head -n 1)
if [ -z "${name}" ]; then
    echo "[ERROR] PROJECT_NAME is empty"
    exit 1
fi

work=/tmp/cpp-logger-retain-work
root=/tmp/cpp-logger-retain-cli
log="${root}/var/${name}/log"
absent=/tmp/cpp-logger-retain-absent
active_dir=/tmp/cpp-logger-retain-active
fail_dir=/tmp/cpp-logger-retain-fail
fake_bin=/tmp/cpp-logger-retain-bin
today=20261110
active="${name}-20261110.log"

rm -rf "${work}" "${root}" "${absent}" "${active_dir}" "${fail_dir}" "${fake_bin}"
mkdir -p "${work}" "${log}" "${active_dir}" "${fail_dir}" "${fake_bin}"

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
g++ -std=c++17 -Wall -Wextra -Werror $(pkg-config --cflags glib-2.0) \
    -I src/main/cpp -c tests/cpp-logger-retain-driver.cpp -o "${work}/driver.o"
g++ -std=c++17 -Wall -Wextra -Werror \
    -o "${work}/cpp-logger" "${work}/main.o" "${work}/logger.o" "${work}/about.o" "${work}/aboutcli.o" "${work}/help.o" "${work}/invocation.o" \
    $(pkg-config --libs glib-2.0) -pthread
g++ -std=c++17 -Wall -Wextra -Werror \
    -o "${work}/retain-driver" "${work}/driver.o" "${work}/logger.o" "${work}/about.o" "${work}/aboutcli.o" "${work}/help.o" "${work}/invocation.o" \
    $(pkg-config --libs glib-2.0) -pthread

bin="${work}/cpp-logger"
driver="${work}/retain-driver"

printf 'kept\n' > "${log}/notes.txt"
printf 'invalid\n' > "${log}/${name}-20260230.log"
printf 'today\n' > "${log}/${name}-20261110.log"
printf 'plain7\n' > "${log}/${name}-20261103.log"
printf 'day8\n' > "${log}/${name}-20261102.log"
printf 'day30\n' > "${log}/${name}-20261011.log"
printf 'day31\n' > "${log}/${name}-20261010.log"
printf 'short\n' > "${log}/${name}-2026.log"

if [ -e "${absent}" ]; then
    echo "[ERROR] absent directory was created early"
    exit 1
fi
"${driver}" "${absent}" "${active}" "${today}"
if [ -e "${absent}" ]; then
    echo "[ERROR] a missing log directory was created"
    exit 1
fi

printf 'stay\n' > "${fail_dir}/${name}-20261010.log"
"${driver}" "${fail_dir}" "${active}" "20260230"
if [ ! -f "${fail_dir}/${name}-20261010.log" ]; then
    echo "[ERROR] an unreadable today deleted a log"
    exit 1
fi
rm -f "${fail_dir}/${name}-20261010.log"

"${driver}" "${log}" "${active}" "${today}"

if [ ! -f "${log}/notes.txt" ] || [ ! -f "${log}/${name}-20260230.log" ] || [ ! -f "${log}/${name}-2026.log" ]; then
    echo "[ERROR] a name that is not a dated log was removed"
    exit 1
fi
if [ ! -f "${log}/${active}" ]; then
    echo "[ERROR] today was not left plain"
    exit 1
fi
if [ -e "${log}/${name}-20261110.log.tar.gz" ] || [ -e "${log}/${name}-20261110.log.zip" ]; then
    echo "[ERROR] today was packaged"
    exit 1
fi
if [ ! -f "${log}/${name}-20261103.log" ]; then
    echo "[ERROR] count 7 lost the plain file"
    exit 1
fi
if [ -e "${log}/${name}-20261103.log.tar.gz" ] || [ -e "${log}/${name}-20261103.log.zip" ]; then
    echo "[ERROR] count 7 was packaged"
    exit 1
fi
if [ -e "${log}/${name}-20261102.log" ]; then
    echo "[ERROR] count 8 kept the plain file"
    exit 1
fi
if [ ! -f "${log}/${name}-20261102.log.tar.gz" ]; then
    echo "[ERROR] count 8 has no tar.gz package"
    exit 1
fi
if [ -e "${log}/${name}-20261102.log.zip" ]; then
    echo "[ERROR] count 8 also wrote a zip package"
    exit 1
fi
if [ -e "${log}/${name}-20261102.log.tar.gz.partial" ]; then
    echo "[ERROR] count 8 left a partial package"
    exit 1
fi
members=$(tar -tzf "${log}/${name}-20261102.log.tar.gz")
if [ "${members}" != "${name}-20261102.log" ]; then
    echo "[ERROR] count 8 package does not hold that one plain file"
    exit 1
fi
if [ -e "${log}/${name}-20261011.log" ]; then
    echo "[ERROR] count 30 kept the plain file"
    exit 1
fi
if [ ! -f "${log}/${name}-20261011.log.tar.gz" ]; then
    echo "[ERROR] count 30 has no package"
    exit 1
fi
if [ -e "${log}/${name}-20261010.log" ] || [ -e "${log}/${name}-20261010.log.tar.gz" ] || [ -e "${log}/${name}-20261010.log.zip" ]; then
    echo "[ERROR] count 31 still has a file"
    exit 1
fi

printf 'active-old\n' > "${active_dir}/${name}-20261010.log"
printf 'other-old\n' > "${active_dir}/${name}-20261009.log"
"${driver}" "${active_dir}" "${name}-20261010.log" "${today}"
if [ ! -f "${active_dir}/${name}-20261010.log" ]; then
    echo "[ERROR] the active file was removed"
    exit 1
fi
if [ -e "${active_dir}/${name}-20261009.log" ] || [ -e "${active_dir}/${name}-20261009.log.tar.gz" ]; then
    echo "[ERROR] a day past 30 days stayed beside the active file"
    exit 1
fi

printf 'keep-me\n' > "${fail_dir}/${name}-20261102.log"
printf '%s\n' '#!/bin/sh' 'exit 1' > "${fake_bin}/tar"
chmod 755 "${fake_bin}/tar"
PATH="${fake_bin}:${PATH}" "${driver}" "${fail_dir}" "${active}" "${today}"
if [ ! -f "${fail_dir}/${name}-20261102.log" ]; then
    echo "[ERROR] a failed package removed the plain file"
    exit 1
fi
if [ -e "${fail_dir}/${name}-20261102.log.tar.gz" ] || [ -e "${fail_dir}/${name}-20261102.log.tar.gz.partial" ]; then
    echo "[ERROR] a failed package left a package behind"
    exit 1
fi

printf 'still-here\n' > "${log}/${name}-20261001.log"
env -u HOME -u PREFIX "${bin}" help >"${work}/help.txt" 2>&1
env -u HOME -u PREFIX "${bin}" about >"${work}/about.txt" 2>&1
list_out=$(env -u HOME -u PREFIX "${bin}" list --prefix "${root}")
if env -u HOME -u PREFIX "${bin}" show --prefix "${root}" --log-datestamp 20261102 >"${work}/show-pack.txt" 2>"${work}/show-pack.err"; then
    echo "[ERROR] show read a day that remains only as a package"
    exit 1
fi
grep -F "cannot read log file ${log}/${name}-20261102.log" "${work}/show-pack.err" >/dev/null
show_plain=$(env -u HOME -u PREFIX "${bin}" show --prefix "${root}" --log-datestamp 20261103)
printf '%s\n' "${show_plain}" | grep -F 'plain7' >/dev/null

if [ ! -f "${log}/${name}-20261001.log" ]; then
    echo "[ERROR] help, about, list, or show removed an old day"
    exit 1
fi
if [ ! -f "${log}/${name}-20261103.log" ] || [ ! -f "${log}/${name}-20261102.log.tar.gz" ]; then
    echo "[ERROR] help, about, list, or show changed the retained days"
    exit 1
fi
printf '%s\n' "${list_out}" | grep -F "${name}-20261001.log" >/dev/null
printf '%s\n' "${list_out}" | grep -F "${name}-20261103.log" >/dev/null
printf '%s\n' "${list_out}" | grep -F "${name}-20261110.log" >/dev/null
if printf '%s\n' "${list_out}" | grep -F 'tar.gz' >/dev/null; then
    echo "[ERROR] list printed a package"
    exit 1
fi
if printf '%s\n' "${list_out}" | grep -F "${name}-20261102.log" >/dev/null; then
    echo "[ERROR] list printed a packaged day"
    exit 1
fi

env -u HOME -u PREFIX "${bin}" test --prefix "${root}" --log-datestamp 20261103 >"${work}/test.txt" 2>&1
if [ -f "${log}/${name}-20261001.log" ]; then
    echo "[ERROR] test left a day past 30 days"
    exit 1
fi
if [ ! -f "${log}/${name}-20261103.log" ] || [ ! -f "${log}/${name}-20261102.log.tar.gz" ]; then
    echo "[ERROR] test removed the active plain file or the count 8 package"
    exit 1
fi
if [ -e "${log}/${name}-20261103.log.tar.gz" ] || [ -e "${log}/${name}-20261103.log.zip" ]; then
    echo "[ERROR] test packaged the active file"
    exit 1
fi

rm -rf "${work}" "${root}" "${absent}" "${active_dir}" "${fail_dir}" "${fake_bin}"
echo "PASS cpp log retention"
