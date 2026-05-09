#!/bin/bash
# SPDX-License-Identifier: MIT
# Copyright (c) 2020-2026 Sam Urmian, Mateus de Oliveira Oliveira, and contributors.


set -u

failures=0
cleanup_dir=""

note() {
    printf '%s\n' "$1"
}

pass() {
    printf '[OK] %s\n' "$1"
}

optional() {
    printf '[OPTIONAL] %s\n' "$1"
}

fail() {
    printf '[MISSING] %s\n' "$1"
    failures=$((failures + 1))
}

version_ge() {
    local lhs="${1#v}"
    local rhs="${2#v}"
    local i max
    local IFS=.
    local lhs_parts=()
    local rhs_parts=()

    read -r -a lhs_parts <<<"$lhs"
    read -r -a rhs_parts <<<"$rhs"

    max="${#lhs_parts[@]}"
    if [ "${#rhs_parts[@]}" -gt "$max" ]; then
        max="${#rhs_parts[@]}"
    fi

    for ((i = 0; i < max; ++i)); do
        local left="${lhs_parts[i]:-0}"
        local right="${rhs_parts[i]:-0}"
        if ((10#$left > 10#$right)); then
            return 0
        fi
        if ((10#$left < 10#$right)); then
            return 1
        fi
    done

    return 0
}

extract_version() {
    printf '%s\n' "$1" | sed -E -n '1s/[^0-9]*([0-9]+([.][0-9]+)+).*/\1/p'
}

check_command() {
    local cmd_path="$1"
    local label="$2"
    if [ -n "$cmd_path" ] && [ -x "$cmd_path" ]; then
        pass "$label found at $cmd_path"
        return 0
    fi

    fail "$label not found"
    return 1
}

find_preferred_tool() {
    local cmd="$1"
    shift
    local candidate

    for candidate in "$@"; do
        if [ -x "$candidate" ]; then
            printf '%s\n' "$candidate"
            return 0
        fi
    done

    command -v "$cmd" 2>/dev/null || return 1
}

check_versioned_tool() {
    local cmd_path="$1"
    local label="$2"
    local minimum="$3"
    local version

    if ! check_command "$cmd_path" "$label"; then
        return 1
    fi

    version="$(extract_version "$("$cmd_path" --version 2>/dev/null | head -n 1)")"
    if [ -z "$version" ]; then
        fail "Could not determine $label version"
        return 1
    fi

    if version_ge "$version" "$minimum"; then
        pass "$label version $version satisfies minimum $minimum"
        return 0
    fi

    fail "$label version $version is below minimum $minimum"
    return 1
}

check_optional_versioned_tool() {
    local cmd_path="$1"
    local label="$2"
    local minimum="$3"
    local version

    if [ -z "$cmd_path" ] || [ ! -x "$cmd_path" ]; then
        optional "$label not found (only needed when regenerating the parser)"
        return 0
    fi

    version="$(extract_version "$("$cmd_path" --version 2>/dev/null | head -n 1)")"
    if [ -z "$version" ]; then
        optional "Could not determine $label version (only needed when regenerating the parser)"
        return 0
    fi

    if version_ge "$version" "$minimum"; then
        optional "$label version $version is available for parser regeneration"
        return 0
    fi

    optional "$label version $version is below recommended $minimum for parser regeneration"
    return 0
}

check_cxx20_compiler() {
    local compiler=""
    local candidate

    for candidate in c++ clang++ g++; do
        if command -v "$candidate" >/dev/null 2>&1; then
            compiler="$candidate"
            break
        fi
    done

    if [ -z "$compiler" ]; then
        fail "No C++ compiler found (looked for c++, clang++, g++)"
        return 1
    fi

    pass "Using C++ compiler: $(command -v "$compiler")"

    cleanup_dir="$(mktemp -d "${TMPDIR:-/tmp}/treewidzard-check.XXXXXX")"
    trap 'if [ -n "${cleanup_dir:-}" ]; then rm -rf "$cleanup_dir"; fi' EXIT
    cat >"$cleanup_dir/test.cpp" <<'EOF'
#include <vector>
int main() {
    std::vector<int> v{1, 2, 3};
    return static_cast<int>(v.size()) - 3;
}
EOF

    if "$compiler" -std=c++20 -c "$cleanup_dir/test.cpp" -o "$cleanup_dir/test.o" >/dev/null 2>&1; then
        pass "$compiler accepts -std=c++20"
        return 0
    fi

    fail "$compiler does not compile a simple C++20 source file"
    return 1
}

check_build_command() {
    check_command "$(find_preferred_tool make || true)" "make"
}

print_hint() {
    local os_name="$1"

    note ""
    case "$os_name" in
        Darwin)
            note "Suggested install commands (macOS):"
            note "  xcode-select --install"
            note "  brew install git cmake"
            ;;
        Linux)
            note "Suggested install command (Ubuntu/Debian):"
            note "  sudo apt-get update && sudo apt-get install -y git cmake build-essential"
            ;;
        *)
            note "This checker is intended for Linux/macOS source builds."
            ;;
    esac
}

print_next_steps() {
    note ""
    note "Next steps:"
    note "  cmake -S . -B out/build/default -DCMAKE_BUILD_TYPE=Release"
    note "  cmake --build out/build/default"
}

os_name="$(uname -s)"
bison_path=""
flex_path=""

case "$os_name" in
    Darwin)
        bison_path="$(find_preferred_tool bison \
            /opt/homebrew/opt/bison/bin/bison \
            /usr/local/opt/bison/bin/bison \
            /opt/local/bin/bison || true)"
        flex_path="$(find_preferred_tool flex \
            /opt/homebrew/opt/flex/bin/flex \
            /usr/local/opt/flex/bin/flex \
            /opt/local/bin/flex || true)"
        ;;
    *)
        bison_path="$(find_preferred_tool bison || true)"
        flex_path="$(find_preferred_tool flex || true)"
        ;;
esac

note "TreeWidzard source-build requirement check"
note "Platform: $os_name"
note ""

check_cxx20_compiler
check_versioned_tool "$(find_preferred_tool cmake || true)" "CMake" "3.18.0"
check_build_command
check_optional_versioned_tool "$bison_path" "Bison" "3.0.0"
check_optional_versioned_tool "$flex_path" "Flex" "2.6.0"

note ""
if [ "$failures" -eq 0 ]; then
    note "Result: this system meets the default source-build requirements."
    print_next_steps
    exit 0
fi

note "Result: this system is missing one or more required source-build tools."
print_hint "$os_name"
note ""
note "Then rerun:"
note "  ./check_requirements.sh"
exit 1
