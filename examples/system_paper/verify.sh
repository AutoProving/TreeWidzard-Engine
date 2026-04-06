#!/bin/bash

set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
cd "$ROOT_DIR"

run_and_expect() {
    local description="$1"
    local expected="$2"
    shift 2

    echo "== $description =="
    local output
    output="$("$@")"
    printf '%s\n' "$output"
    if [[ "$output" != *"$expected"* ]]; then
        echo "Expected output to contain: $expected" >&2
        exit 1
    fi
    echo
}

run_and_expect \
    "ATP five-colorable with ParallelBreadthFirstSearch" \
    "PROPERTY SATISFIED" \
    ./treewidzard.sh -atp tw=4 -pl ParallelBreadthFirstSearch \
    examples/system_paper/atp/five_colorable.txt

run_and_expect \
    "ATP four-colorable with ParallelBreadthFirstSearch" \
    "PROPERTY NOT SATISFIED" \
    ./treewidzard.sh -atp tw=4 -pl ParallelBreadthFirstSearch \
    examples/system_paper/atp/four_colorable.txt

run_and_expect \
    "ATP five-colorable with ParallelIsomorphismBreadthFirstSearch" \
    "PROPERTY SATISFIED" \
    ./treewidzard.sh -atp tw=4 -pl ParallelIsomorphismBreadthFirstSearch \
    examples/system_paper/atp/five_colorable.txt

run_and_expect \
    "ATP premise-pruning example" \
    "PROPERTY SATISFIED" \
    ./treewidzard.sh -atp tw=4 -premise BreadthFirstSearch \
    examples/system_paper/atp/vertex_count_bounded.txt

run_and_expect \
    "PACE model checking of 3-colorability on the diamond graph" \
    "PROPERTY SATISFIED" \
    ./treewidzard.sh -modelcheck PACE \
    examples/system_paper/model_checking/three_colorable.txt \
    examples/system_paper/model_checking/diamond.gr \
    examples/system_paper/model_checking/diamond.td

run_and_expect \
    "ITD model checking of 3-colorability on the diamond graph" \
    "PROPERTY SATISFIED" \
    ./treewidzard.sh -modelcheck ITD \
    examples/system_paper/model_checking/three_colorable.txt \
    examples/system_paper/model_checking/diamond.itd

run_and_expect \
    "PACE invariant computation on the diamond graph" \
    "Invariant value: INV(x)=2" \
    ./treewidzard.sh -modelcheck PACE \
    examples/system_paper/invariants/independence_number.txt \
    examples/system_paper/model_checking/diamond.gr \
    examples/system_paper/model_checking/diamond.td

echo "All system paper examples verified."
