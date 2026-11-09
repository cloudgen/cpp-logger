#!/bin/bash
# =========================================================================
# build.sh - Build Script for cpp-logger
# Minimal change: Sources project.sh as Single Source of Truth + safe fallbacks
# =========================================================================

set -e

# ====================== Source project.sh (Single Source of Truth) ======================
if [ -f "./project.sh" ]; then
    echo "[INFO] Sourcing project.sh as Single Source of Truth..."
    source ./project.sh
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
BINARY="${BUILD_DIR}/${PROJECT_NAME}"

install_deps() {
    echo "[INFO] Installing required development packages..."
    sudo apt update -qq
    sudo apt install -y -qq \
        build-essential \
        pkg-config \
        libglib2.0-dev

    echo "[INFO] All dependencies installed successfully."
}

# Place the already-built binary. Does not compile, does not call sudo, does not run apt.
place_built_binary() {
    local dest_dir="$1"

    if [ ! -x "${BINARY}" ]; then
        echo "[ERROR] Build the program first. Missing ${BINARY}"
        exit 1
    fi
    mkdir -p "${dest_dir}"
    install -m 0755 "${BINARY}" "${dest_dir}/${PROJECT_NAME}"
    echo "[INFO] Installed ${dest_dir}/${PROJECT_NAME}"
}

install_local() {
    # Operator place only. Tests must not call this and must not expand HOME.
    if [ -z "${HOME}" ]; then
        echo "[ERROR] HOME is empty. Local installation needs a home directory."
        exit 1
    fi
    place_built_binary "${HOME}/.local/bin"
}

install_global() {
    local prefix="${PREFIX:-/usr/local}"
    local global_bin="${prefix%/}/bin"
    if [ ! -d "${global_bin}" ] || [ ! -w "${global_bin}" ]; then
        echo "[ERROR] ${global_bin} is not writable. Re-run as the user who can write that directory. This command does not call sudo."
        exit 1
    fi
    place_built_binary "${global_bin}"
}

# ====================== clean ======================
clean() {
    echo "[INFO] Cleaning build artifacts..."
    rm -rf "${BUILD_DIR}" target/ build/ .gradle/ autom4te.cache 2>/dev/null || true
    echo "[INFO] Clean completed."
}

# ====================== build ======================
# =========================================================================
# CIAO PROTECTION BLOCK - build() function (Sacred)
# =========================================================================
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
# =========================================================================
build() {
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

    echo "[INFO] Build successful!"
    echo "[INFO] Binary ready → ${BINARY}"
    echo "[INFO] Run with: ${BINARY}"
}

# ====================== Main logic ======================
case "$1" in
    install|deps)
        install_deps
        ;;
    install-local)
        install_local
        ;;
    install-global)
        install_global
        ;;
    clean)
        clean
        ;;
    build|""|simple)
        build
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