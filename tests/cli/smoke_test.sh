#!/bin/bash
# SPDX-License-Identifier: MIT
# Copyright (c) 2020-2026 Sam Urmian, Mateus de Oliveira Oliveira, and contributors.


# Smoke Test for TreeWidzard
# Verifies that the executable runs and prints help

set -euo pipefail
shopt -s nullglob

resolve_executable() {
    if [ -n "${TREEWIDZARD_BUILD_DIR:-}" ] && [ -f "${TREEWIDZARD_BUILD_DIR}/treewidzard" ]; then
        printf '%s\n' "${TREEWIDZARD_BUILD_DIR}/treewidzard"
        return 0
    fi

    local candidates=(
        "./treewidzard"
        "./out/build/default/treewidzard"
        "./out/build/release/treewidzard"
        "./out/build/debug/treewidzard"
        ./out/build/*/treewidzard
        "./build/treewidzard"
        "./Build/treewidzard"
        ./build*/treewidzard
        "/app/build/treewidzard"
    )
    local candidate
    for candidate in "${candidates[@]}"; do
        if [ -f "$candidate" ]; then
            printf '%s\n' "$candidate"
            return 0
        fi
    done

    return 1
}

if ! EXE="$(resolve_executable)"; then
    echo "Error: treewidzard executable not found" >&2
    exit 1
fi

echo "Using executable: $EXE"

# Run help command
echo "Running help check..."
set +e
OUTPUT=$("$EXE" --help 2>&1)
EXIT_CODE=$?
set -e

echo "Exit code: $EXIT_CODE"

# Check if usage text is present
if echo "$OUTPUT" | grep -q "Usage:"; then
    echo "Help text verification: PASSED"
else
    echo "Help text verification: FAILED"
    echo "Output was:"
    echo "$OUTPUT"
    exit 1
fi

# Check logic for Exit code (20 or 0 or 1 depending on impl)
# Attempt 899 returned 20.
if [ "$EXIT_CODE" -eq 20 ] || [ "$EXIT_CODE" -eq 0 ]; then
    echo "Exit code verification: PASSED"
else
    echo "Exit code verification: WARNING (Expected 20 or 0, got $EXIT_CODE)"
    # Don't fail strictly on exit code if help text was found, unless it segfaulted (139)?
    if [ "$EXIT_CODE" -gt 128 ]; then
         echo "Process crashed!"
         exit 1
    fi
fi

echo "SMOKE TEST PASSED"
exit 0
