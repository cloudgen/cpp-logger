#!/bin/sh
# =============================================================================
# build.sh
# =============================================================================
#
# GENERAL PURPOSE:
# Compile and place this program. The interpreter is requirement-cpp-build-sh.md.
# The function names and headers are requirement-cpp-build-sh-coding-style.md.
#
# CIAO PRINCIPLES APPLIED:
# - Caution: set -e stops the script when a compile command fails.
# - Intentional: typed verbs stay short. Functions keep the build_ prefix.
#
# Last reviewed: 2026-11-10
# =============================================================================

set -e

# Read project.sh. A missing file keeps the fallback names.
if [ -f "./project.sh" ]; then
    echo "[INFO] Reading project.sh as Single Source of Truth..."
    . ./project.sh
else
    echo "[WARN] project.sh not found — using safe generic fallbacks"
    export PRJ="${PRJ:-$(basename "$(pwd)")}"
    export SRC="${SRC:-src}"
    export SRC_C="${SRC_C:-$SRC/main/cpp}"
    export BUILD="${BUILD:-target}"
    export BUILD_DEBUG="${BUILD_DEBUG:-$BUILD/exe/debug}"
fi

# Ensure required variables have sensible defaults
PROJECT_NAME="${PRJ:-app}"
SRC_DIR="${SRC_C:-src/main/cpp}"
BUILD_DIR="${BUILD_DEBUG:-target/exe/debug}"
EXE_SUFFIX=""
HOST_UNAME=$(uname -s 2>/dev/null || printf '%s' '')
case "${HOST_UNAME}" in
    MINGW*|MSYS*|CYGWIN*|Windows_NT)
        EXE_SUFFIX=".exe"
        ;;
esac
if [ "${OS:-}" = "Windows_NT" ]; then
    EXE_SUFFIX=".exe"
fi
BINARY="${BUILD_DIR}/${PROJECT_NAME}${EXE_SUFFIX}"
DOWNLOAD_DIR="download"
DOWNLOAD_BINARY="${DOWNLOAD_DIR}/${PROJECT_NAME}${EXE_SUFFIX}"

# =============================================================================
# build_install_deps() - Install the apt packages the compile needs
# =============================================================================
#
# GENERAL PURPOSE:
# Ask apt for the compiler, pkg-config, and the GLib headers. The package
# names stay in requirement-cpp-dependencies.md.
#
# CIAO PRINCIPLES APPLIED:
# - Caution: set -e stops the script when apt fails.
# - Intentional: this is developer bootstrap. It does not copy the binary.
#
# Last reviewed: 2026-11-10
# =============================================================================
build_install_deps() {
    echo "[INFO] Installing required development packages..."
    sudo apt update -qq
    sudo apt install -y -qq \
        build-essential \
        pkg-config \
        libglib2.0-dev

    echo "[INFO] All dependencies installed successfully."
}

# =============================================================================
# build_place_binary() - Copy the built program into one directory
# =============================================================================
#
# GENERAL PURPOSE:
# Copy the already-built binary to the directory named by the caller and
# set its mode. This function does not compile, does not call sudo, and
# does not run apt.
#
# CIAO PRINCIPLES APPLIED:
# - Caution: a missing binary stops the command before mkdir.
# - Intentional: local and global place share this one copy.
#
# Last reviewed: 2026-11-10
# =============================================================================
build_place_binary() {
    local dest_dir="$1"

    if [ ! -x "${BINARY}" ]; then
        echo "[ERROR] Build the program first. Missing ${BINARY}"
        exit 1
    fi
    mkdir -p "${dest_dir}"
    install -m 0755 "${BINARY}" "${dest_dir}/${PROJECT_NAME}"
    echo "[INFO] Installed ${dest_dir}/${PROJECT_NAME}"
}

# =============================================================================
# build_install_local() - Place the binary in the user bin
# =============================================================================
#
# GENERAL PURPOSE:
# Copy the built program to the user bin under HOME. Tests must not call
# this function and must not expand HOME.
#
# CIAO PRINCIPLES APPLIED:
# - Caution: an empty HOME stops the command.
# - Intentional: the place is the user bin, not the log directory.
#
# Last reviewed: 2026-11-10
# =============================================================================
build_install_local() {
    if [ -z "${HOME}" ]; then
        echo "[ERROR] HOME is empty. Local installation needs a home directory."
        exit 1
    fi
    build_place_binary "${HOME}/.local/bin"
}

# =============================================================================
# build_install_global() - Place the binary in the prefix bin
# =============================================================================
#
# GENERAL PURPOSE:
# Copy the built program to the prefix bin. PREFIX defaults to /usr/local.
# This command does not call sudo.
#
# CIAO PRINCIPLES APPLIED:
# - Caution: a missing or unwritable directory stops the command.
# - Intentional: the operator who can write that directory runs this verb.
#
# Last reviewed: 2026-11-10
# =============================================================================
build_install_global() {
    local prefix="${PREFIX:-/usr/local}"
    local global_bin="${prefix%/}/bin"
    if [ ! -d "${global_bin}" ] || [ ! -w "${global_bin}" ]; then
        echo "[ERROR] ${global_bin} is not writable. Re-run as the user who can write that directory. This command does not call sudo."
        exit 1
    fi
    build_place_binary "${global_bin}"
}

# =============================================================================
# build_clean() - Remove build output
# =============================================================================
#
# GENERAL PURPOSE:
# Remove the debug directory and the other build directories. The sources
# stay.
#
# CIAO PRINCIPLES APPLIED:
# - Caution: a missing directory does not fail the clean.
# - Intentional: clean removes build output and leaves the sources.
#
# Last reviewed: 2026-11-11
# =============================================================================
build_clean() {
    echo "[INFO] Cleaning build artifacts..."
    rm -rf "${BUILD_DIR}" "${DOWNLOAD_DIR}" target/ build/ .gradle/ autom4te.cache 2>/dev/null || true
    echo "[INFO] Clean completed."
}

# =============================================================================
# build_link() - Compile and link the program
# =============================================================================
#
# GENERAL PURPOSE:
# This function is the single place that compiles and links.
#
# CIAO PRINCIPLES APPLIED:
# - Caution: a tree with no source file stops before the compiler runs.
# - Intentional: C and C++ stay in one link. The flags stay on these lines.
# - Over-protect: the protection block below stays with this function.
#
# Last reviewed: 2026-11-11
# =============================================================================
# =============================================================================
# CIAO PROTECTION BLOCK - build_link() function (Sacred)
# =============================================================================
#
# This function is the Single Point of Truth for compilation and linking.
# Future AI or developers MUST NOT:
#   - Rewrite or heavily refactor the compilation logic
#   - Remove or simplify the PKG_FLAGS
#   - Change the compile + link approach without strong justification
#   - Bypass this function for building the application
#
# Reason: This logic was battle-tested and finally produced a successful build.
# Changing it violates CIAO Principle 7 (Reusable Function Protection) and
# Principle 18 (Protect Against AI & Human Modification).
#
# Last updated: 2026-05-24
# =============================================================================
build_link() {
    echo "[INFO] Starting simple GCC/C++ build for ${PROJECT_NAME}..."

    mkdir -p "${BUILD_DIR}"

    echo "[DEBUG] PKG_CONFIG_PATH = ${PKG_CONFIG_PATH}"
    echo "[DEBUG] Packages requested from pkg-config: glib-2.0"

    PKG_FLAGS=$(pkg-config --cflags --libs \
        glib-2.0 2>&1)

    echo "[DEBUG] pkg-config output:"
    echo "${PKG_FLAGS}"

    C_FILES=$(find "${SRC_DIR}" -name "*.c" | tr '\n' ' ')
    CPP_FILES=$(find "${SRC_DIR}" -name "*.cpp" | tr '\n' ' ')

    echo "[DEBUG] Found C files  : ${C_FILES}"
    echo "[DEBUG] Found C++ files: ${CPP_FILES}"

    if [ -z "$C_FILES$CPP_FILES" ]; then
        echo "[ERROR] No source files found!"
        exit 1
    fi

    echo "[INFO] Compiling..."
    if [ -n "$C_FILES" ]; then
        gcc -std=c11 -Wall -Wextra -g -O2 ${PKG_FLAGS} -c ${C_FILES}
    fi
    if [ -n "$CPP_FILES" ]; then
        g++ -std=c++17 -Wall -Wextra -g -O2 ${PKG_FLAGS} -c ${CPP_FILES}
    fi

    echo "[INFO] Linking final binary..."
    if [ -n "$CPP_FILES" ]; then
        g++ *.o -o "${BINARY}" ${PKG_FLAGS} -pthread
    else
        gcc *.o -o "${BINARY}" ${PKG_FLAGS} -pthread
    fi

    rm -f *.o

    mkdir -p "${DOWNLOAD_DIR}"
    cp "${BINARY}" "${DOWNLOAD_BINARY}"

    echo "[INFO] Build successful!"
    echo "[INFO] Binary ready → ${BINARY}"
    echo "[INFO] Download ready → ${DOWNLOAD_BINARY}"
    echo "[INFO] Run with: ${BINARY}"
}

# ====================== Main logic ======================
case "$1" in
    install|deps)
        build_install_deps
        ;;
    install-local)
        build_install_local
        ;;
    install-global)
        build_install_global
        ;;
    clean)
        build_clean
        ;;
    build|""|simple)
        build_link
        ;;
    *)
        echo "Usage: $0 {install|install-local|install-global|clean|build}"
        echo "   install         → install system dependencies"
        echo "   install-local   → copy the built binary to \$HOME/.local/bin"
        echo "   install-global  → copy the built binary to \${PREFIX:-/usr/local}/bin"
        echo "   clean           → remove build artifacts"
        echo "   build           → compile the application"
        exit 1
        ;;
esac
