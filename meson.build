# =========================================================================
# meson.build - CIAO DEFENSIVE EDITION
# =========================================================================
#
# !!! THIS FILE IS PART OF THE CIAO DEFENSIVE PROGRAMMING STYLE !!!
# !!! DO NOT SIMPLIFY, REMOVE COMMENTS, OR "CLEAN UP" THIS FILE !!!
#
# Purpose:
#   Provides Meson build support for the C and C++ sources.
# Parameters:
#   none
# Dependencies:
#   PRJ from project.ini. executable() uses that value.
# Why This Design:
#   One file keeps the Meson build. The product id stays in project.ini.
# CIAO Principles Applied:
#   Intentional: every build statement has Purpose and Protection Rule.
# Protection Rule:
#   DO NOT hardcode the project name past the project() literal. DO NOT remove these comments.
# =========================================================================
#
# Setup and compile:
#   meson setup target/exe/debug
#   meson compile -C target/exe/debug

project('cpp-logger', 'c', 'cpp',
    version: '1.1.1',
    license: 'MIT',
    default_options: [
        'c_std=c11',
        'cpp_std=c++17',
        'warning_level=2',
        'buildtype=debugoptimized',
    ]
)

fs = import('fs')

prj = meson.project_name()
if fs.exists('project.ini')
    foreach line : fs.read('project.ini').split('\n')
        if line.startswith('PRJ=')
            prj = line.substring(4).strip()
        endif
    endforeach
endif

host_name = host_machine.system()
is_windows = host_name == 'windows'
is_darwin = host_name == 'darwin'
is_linux = host_name == 'linux'

libc_name = 'glibc'
if is_linux
    cc = meson.get_compiler('c')
    uclibc_probe = '''#include <features.h>
#ifndef __UCLIBC__
#error not uclibc
#endif
'''
    glibc_probe = '''#include <features.h>
#ifndef __GLIBC__
#error not glibc
#endif
'''
    if cc.compiles(uclibc_probe, name: 'uclibc')
        libc_name = 'uclibc'
    elif cc.compiles(glibc_probe, name: 'glibc')
        libc_name = 'glibc'
    else
        libc_name = 'musl'
    endif
endif
message('Target host: ' + host_name + ' libc: ' + libc_name)

# =========================================================================
# dependency glib-2.0
#
# Purpose:
#   Supplies the GLib compile and link arguments on Linux.
# Parameters:
#   none
# Dependencies:
#   The host is linux. Windows and macOS skip this call.
# Why This Design:
#   This is the Meson form of a separate pkg-config --cflags and pkg-config --libs.
# CIAO Principles Applied:
#   Caution: the module list stays glib-2.0.
# Protection Rule:
#   DO NOT request a GUI toolkit module.
# =========================================================================
# =========================================================================
# dependency threads
#
# Purpose:
#   Adds the POSIX thread link flag on Linux.
# Parameters:
#   none
# Dependencies:
#   The host is linux.
# Why This Design:
#   The link line adds -pthread.
# CIAO Principles Applied:
#   Caution: Windows and macOS skip this call.
# Protection Rule:
#   DO NOT drop the thread dependency on Linux.
# =========================================================================
if is_linux
    glib_dep = dependency('glib-2.0', required: true)
    thread_dep = dependency('threads', required: true)
else
    glib_dep = declare_dependency()
    thread_dep = declare_dependency()
endif

sources = []
cpp_out = run_command('sh', '-c', 'find src/main/cpp -name "*.cpp" 2>/dev/null || true', check: false).stdout().strip()
c_out = run_command('sh', '-c', 'find src/main/c -name "*.c" 2>/dev/null || true', check: false).stdout().strip()
if cpp_out != ''
    foreach rel : cpp_out.split('\n')
        if rel != ''
            sources += files(rel)
        endif
    endforeach
endif
if c_out != ''
    foreach rel : c_out.split('\n')
        if rel != ''
            sources += files(rel)
        endif
    endforeach
endif

# =========================================================================
# executable
#
# Purpose:
#   Builds the debug binary and does not launch it.
# Parameters:
#   none
# Dependencies:
#   dependency('glib-2.0') and dependency('threads') on Linux.
# Why This Design:
#   One executable is the build. run_target depends on it.
# CIAO Principles Applied:
#   Anti-fragile: the binary name is the PRJ value.
# Protection Rule:
#   DO NOT rename the run target. DO NOT hardcode the project name past project().
# =========================================================================
exe = executable(prj, sources,
    dependencies: [
        glib_dep,
        thread_dep,
    ],
    install: true,
    install_dir: get_option('bindir')
)

# =========================================================================
# run
#
# Purpose:
#   Builds the executable and then executes it.
# Parameters:
#   none
# Dependencies:
#   executable()
# Why This Design:
#   run_target('run') is the Meson form of Ant run depending on build.
# CIAO Principles Applied:
#   Anti-fragile: the target depends on the executable.
# Protection Rule:
#   DO NOT rename this target. DO NOT add a host task name.
# =========================================================================
run_target('run',
    command: [exe],
    depends: exe
)

if not fs.exists('project.sh')
    run_command('sh', '-c', 'printf "%s\n" "export PRJ=$1" "export SRC=src" "export SRC_C=$SRC/main/c" "export SRC_CPP=$SRC/main/cpp" > project.sh && chmod 755 project.sh', 'gen', prj, check: true)
endif
