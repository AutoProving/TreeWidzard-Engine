#!/bin/bash
# TreeWidzard wrapper script - makes it easier to use TreeWidzard

set -euo pipefail
shopt -s nullglob

# Get the directory where this script is located
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

resolve_build_dir() {
    if [ -n "${TREEWIDZARD_BUILD_DIR:-}" ] && [ -f "${TREEWIDZARD_BUILD_DIR}/treewidzard" ]; then
        printf '%s\n' "${TREEWIDZARD_BUILD_DIR}"
        return 0
    fi

    local candidates=(
        "$SCRIPT_DIR/build"
        "$SCRIPT_DIR/Build"
        "$SCRIPT_DIR/build_port"
        "$SCRIPT_DIR"/build*
    )
    local candidate
    for candidate in "${candidates[@]}"; do
        if [ -f "$candidate/treewidzard" ]; then
            printf '%s\n' "$candidate"
            return 0
        fi
    done

    return 1
}

if ! BUILD_DIR="$(resolve_build_dir)"; then
    echo "Error: treewidzard executable not found in any known build directory." >&2
    echo "Set TREEWIDZARD_BUILD_DIR or build the project with: cmake -S . -B build && cmake --build build" >&2
    exit 1
fi

# Set up environment
export TREEWIDZARD_SEARCHSTRATEGIES="$BUILD_DIR/SearchStrategies"
export TREEWIDZARD_DPCORES="$BUILD_DIR/DPCores"

# Run treewidzard with all arguments passed through
"$BUILD_DIR/treewidzard" "$@"
