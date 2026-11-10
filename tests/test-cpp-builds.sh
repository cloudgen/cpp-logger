#!/bin/sh
# TP-CPP-ANT TP-CPP-ANT-STYLE TP-CPP-GRADLE TP-CPP-GRADLE-STYLE
# TP-CPP-MESON TP-CPP-MESON-STYLE
# Reads build.xml, build.gradle, and meson.build.
# Runs ant compile and gradle assemble. Does not run meson or a package install.
# Does not call mktemp. Does not read, assign, or delete HOME.
set -eu

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

labels_in_order() {
    id=$1
    file=$2
    purpose=$(grep -n -F 'Purpose' "$file" | head -n 1 | cut -d: -f1)
    parameters=$(grep -n -F 'Parameters' "$file" | head -n 1 | cut -d: -f1)
    dependencies=$(grep -n -F 'Dependencies' "$file" | head -n 1 | cut -d: -f1)
    why=$(grep -n -F 'Why This Design' "$file" | head -n 1 | cut -d: -f1)
    ciao=$(grep -n -F 'CIAO Principles Applied' "$file" | head -n 1 | cut -d: -f1)
    protection=$(grep -n -F 'Protection Rule' "$file" | head -n 1 | cut -d: -f1)
    if [ -z "$purpose" ] || [ -z "$parameters" ] || [ -z "$dependencies" ] || [ -z "$why" ] || [ -z "$ciao" ] || [ -z "$protection" ]; then
        echo "[FAIL] ${id} missing a banner label in ${file}"
        fail=1
        return
    fi
    if [ "$purpose" -ge "$parameters" ] || [ "$parameters" -ge "$dependencies" ] || [ "$dependencies" -ge "$why" ] || [ "$why" -ge "$ciao" ] || [ "$ciao" -ge "$protection" ]; then
        echo "[FAIL] ${id} banner labels are out of order in ${file}"
        fail=1
    fi
}

# TP-CPP-ANT
need TP-CPP-ANT build.xml 'project.ini'
need TP-CPP-ANT build.xml 'cpp-logger'
need TP-CPP-ANT build.xml 'os.windows'
need TP-CPP-ANT build.xml 'os.mac'
need TP-CPP-ANT build.xml 'os.linux'
need TP-CPP-ANT build.xml 'glibc'
need TP-CPP-ANT build.xml 'uclibc'
need TP-CPP-ANT build.xml 'musl'
need TP-CPP-ANT build.xml 'pkg-config'
if ! awk '
    /<exec executable="pkg-config"/ { inexec=1; hasc=0; hasl=0 }
    inexec && /--cflags/ { hasc=1 }
    inexec && /--libs/ { hasl=1 }
    /<\/exec>/ && inexec {
        if (hasc && hasl) combined=1
        if (hasc && !hasl) c++
        if (hasl && !hasc) l++
        inexec=0
    }
    END { if (combined || c < 1 || l < 1) exit 1 }
' build.xml; then
    echo "[FAIL] TP-CPP-ANT pkg-config --cflags and --libs are not separate commands"
    fail=1
fi
need TP-CPP-ANT build.xml '-std=c++17'
need TP-CPP-ANT build.xml '-Wall'
need TP-CPP-ANT build.xml '-Wextra'
need TP-CPP-ANT build.xml '-g'
need TP-CPP-ANT build.xml '-O2'
need TP-CPP-ANT build.xml '-std=c11'
need TP-CPP-ANT build.xml 'target/exe/debug'
need TP-CPP-ANT build.xml 'SRC_C=$SRC/main/c'
need TP-CPP-ANT build.xml 'SRC_CPP=$SRC/main/cpp'
forbid TP-CPP-ANT build.xml 'gtk+-3.0'
forbid TP-CPP-ANT build.xml 'webkit2gtk-4.1'
run_line=$(grep -E '<target[^>]*name="run"' build.xml | head -n 1)
case "${run_line}" in
    *'depends="build"'*) ;;
    *)
        echo "[FAIL] TP-CPP-ANT run does not depend on build"
        fail=1
        ;;
esac

# TP-CPP-ANT-STYLE
need TP-CPP-ANT-STYLE build.xml 'build.xml - CIAO DEFENSIVE EDITION'
labels_in_order TP-CPP-ANT-STYLE build.xml
for verb in build compile clean install run prepare make build-windows build-macos; do
    if ! grep -E -q "name=\"${verb}\"[^>]*description=" build.xml && ! grep -E -q "description=\"[^\"]*\"[^>]*name=\"${verb}\"" build.xml; then
        echo "[FAIL] TP-CPP-ANT-STYLE target ${verb} has no description"
        fail=1
    fi
done
if grep -E -q '<project[^>]*name="cpp-logger"' build.xml; then
    echo "[FAIL] TP-CPP-ANT-STYLE project name attribute is the product id"
    fail=1
fi
need TP-CPP-ANT-STYLE build.xml '${app.name}'

# TP-CPP-GRADLE
need TP-CPP-GRADLE build.gradle 'project.ini'
need TP-CPP-GRADLE build.gradle 'cpp-logger'
need TP-CPP-GRADLE build.gradle 'windows'
need TP-CPP-GRADLE build.gradle 'mac'
need TP-CPP-GRADLE build.gradle 'linux'
need TP-CPP-GRADLE build.gradle 'glibc'
need TP-CPP-GRADLE build.gradle 'uclibc'
need TP-CPP-GRADLE build.gradle 'musl'
if grep -F -q 'pkg-config --cflags --libs' build.gradle || grep -F -q 'pkg-config --libs --cflags' build.gradle; then
    echo "[FAIL] TP-CPP-GRADLE pkg-config --cflags and --libs are one command"
    fail=1
fi
if ! grep -F -q 'pkg-config --cflags' build.gradle || ! grep -F -q 'pkg-config --libs' build.gradle; then
    echo "[FAIL] TP-CPP-GRADLE missing a split pkg-config --cflags or --libs call"
    fail=1
fi
need TP-CPP-GRADLE build.gradle '-std=c++17'
need TP-CPP-GRADLE build.gradle '-Wall'
need TP-CPP-GRADLE build.gradle '-Wextra'
need TP-CPP-GRADLE build.gradle '-g'
need TP-CPP-GRADLE build.gradle '-O2'
need TP-CPP-GRADLE build.gradle 'target/exe/debug'
if ! grep -F -q 'SRC_C=$SRC/main/cpp' build.gradle && ! grep -F -q 'SRC_C=\$SRC/main/cpp' build.gradle; then
    echo "[FAIL] TP-CPP-GRADLE generated SRC_C is not \$SRC/main/cpp"
    fail=1
fi
if ! grep -F -q 'SRC_CPP=$SRC/main/cpp' build.gradle && ! grep -F -q 'SRC_CPP=\$SRC/main/cpp' build.gradle; then
    echo "[FAIL] TP-CPP-GRADLE generated SRC_CPP is not \$SRC/main/cpp"
    fail=1
fi
need TP-CPP-GRADLE build.gradle 'buildWindows'
need TP-CPP-GRADLE build.gradle 'buildMac'
forbid TP-CPP-GRADLE build.gradle 'gtk+-3.0'
forbid TP-CPP-GRADLE build.gradle 'webkit2gtk-4.1'
if ! grep -F -q "dependsOn 'assemble'" build.gradle && ! grep -F -q 'dependsOn("assemble")' build.gradle && ! grep -F -q 'dependsOn assemble' build.gradle; then
    echo "[FAIL] TP-CPP-GRADLE run does not depend on assemble"
    fail=1
fi

# TP-CPP-GRADLE-STYLE
need TP-CPP-GRADLE-STYLE build.gradle 'cpp-logger build.gradle'
labels_in_order TP-CPP-GRADLE-STYLE build.gradle
# help, tasks, properties, dependencies, clean, assemble, build, and check
# come with Gradle. This check names the product tasks the file must define.
for verb in run scriptVersion installDependencies buildWindows buildMac; do
    if ! grep -F -q "$verb" build.gradle; then
        echo "[FAIL] TP-CPP-GRADLE-STYLE missing product task ${verb}"
        fail=1
    fi
done
if [ -f gradlew ]; then
    echo "[FAIL] TP-CPP-GRADLE-STYLE gradlew is present"
    fail=1
fi

# TP-CPP-MESON
need TP-CPP-MESON meson.build 'project.ini'
need TP-CPP-MESON meson.build 'cpp-logger'
need TP-CPP-MESON meson.build 'windows'
need TP-CPP-MESON meson.build 'darwin'
need TP-CPP-MESON meson.build 'linux'
need TP-CPP-MESON meson.build 'glibc'
need TP-CPP-MESON meson.build 'uclibc'
need TP-CPP-MESON meson.build 'musl'
need TP-CPP-MESON meson.build "dependency('glib-2.0'"
need TP-CPP-MESON meson.build "dependency('threads'"
need TP-CPP-MESON meson.build 'c_std=c11'
need TP-CPP-MESON meson.build 'cpp_std=c++17'
need TP-CPP-MESON meson.build 'warning_level=2'
need TP-CPP-MESON meson.build 'buildtype=debugoptimized'
need TP-CPP-MESON meson.build 'target/exe/debug'
need TP-CPP-MESON meson.build 'SRC_C=$SRC/main/c'
need TP-CPP-MESON meson.build 'SRC_CPP=$SRC/main/cpp'
need TP-CPP-MESON meson.build "run_target('run'"
forbid TP-CPP-MESON meson.build 'gtk+-3.0'
forbid TP-CPP-MESON meson.build 'webkit2gtk-4.1'
forbid TP-CPP-MESON meson.build 'build-windows'
forbid TP-CPP-MESON meson.build 'buildWindows'

# TP-CPP-MESON-STYLE
need TP-CPP-MESON-STYLE meson.build 'meson.build - CIAO DEFENSIVE EDITION'
labels_in_order TP-CPP-MESON-STYLE meson.build
# setup, compile, clean, test, introspect, install, and run are the meson
# commands. The file's product target is run. Host task names stay out.
if ! grep -F -q "run_target('run'" meson.build; then
    echo "[FAIL] TP-CPP-MESON-STYLE product target is not run"
    fail=1
fi

# Live compile. A missing src/main/c must be reported and must not fail the build.
ant_log=/tmp/cpp-logger-ant-check.log
gradle_log=/tmp/cpp-logger-gradle-check.log
project_before=/tmp/cpp-logger-project-sh-before
cp -a project.sh "${project_before}"

if [ -d src/main/c ]; then
    echo "[FAIL] TP-CPP-ANT src/main/c exists before the live compile"
    fail=1
fi

if ! env -u HOME ant compile > "${ant_log}" 2>&1; then
    echo "[FAIL] TP-CPP-ANT ant compile exited non-zero"
    cat "${ant_log}"
    fail=1
fi
if ! grep -F -q 'Check src/main/c: absent. Contributes no files.' "${ant_log}"; then
    echo "[FAIL] TP-CPP-ANT ant compile did not report src/main/c absent"
    fail=1
fi
if ! grep -F -q 'Check src/main/cpp: present' "${ant_log}"; then
    echo "[FAIL] TP-CPP-ANT ant compile did not report src/main/cpp present"
    fail=1
fi
if grep -F -q 'src/main/c does not exist' "${ant_log}"; then
    echo "[FAIL] TP-CPP-ANT ant compile still dies on a missing src/main/c"
    fail=1
fi

if ! env -u HOME gradle assemble --offline --console=plain > "${gradle_log}" 2>&1; then
    echo "[FAIL] TP-CPP-GRADLE gradle assemble exited non-zero"
    cat "${gradle_log}"
    fail=1
fi
if ! grep -F -q 'Check src/main/c: absent. Contributes no files.' "${gradle_log}"; then
    echo "[FAIL] TP-CPP-GRADLE gradle assemble did not report src/main/c absent"
    fail=1
fi
if ! grep -F -q 'Check src/main/cpp: present' "${gradle_log}"; then
    echo "[FAIL] TP-CPP-GRADLE gradle assemble did not report src/main/cpp present"
    fail=1
fi
if grep -F -q 'src/main/c does not exist' "${gradle_log}"; then
    echo "[FAIL] TP-CPP-GRADLE gradle assemble still dies on a missing src/main/c"
    fail=1
fi

if [ -d src/main/c ]; then
    echo "[FAIL] TP-CPP-ANT live compile created src/main/c"
    fail=1
fi
if ! cmp -s project.sh "${project_before}"; then
    echo "[FAIL] TP-CPP-ANT live compile changed project.sh"
    fail=1
fi

rm -f "${ant_log}" "${gradle_log}" "${project_before}"

if [ "$fail" -eq 0 ]; then
    echo "PASS cpp builds"
else
    echo "FAIL cpp builds"
    exit 1
fi
