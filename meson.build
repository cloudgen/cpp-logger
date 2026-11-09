# meson.build — second build.
# Compiles the same sources as ./build.sh.
# project() version matches the runtime macros in src/main/cpp/project.h (1.0.8).

project('cpp-logger', 'cpp',
    version: '1.0.8',
    license: 'MIT',
    default_options: [
        'warning_level=3',
        'buildtype=debugoptimized',
        'cpp_std=c++17',
    ]
)

glib_dep = dependency('glib-2.0', required: true)
thread_dep = dependency('threads', required: true)

src_files = files(
    'src/main/cpp/main.cpp',
    'src/main/cpp/logger.cpp',
)

executable('cpp-logger',
    sources: src_files,
    dependencies: [
        glib_dep,
        thread_dep,
    ],
    install: true,
    install_dir: get_option('bindir')
)
