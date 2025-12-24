#!/bin/bash
# TreeWidzard wrapper script - makes it easier to use TreeWidzard

# Get the directory where this script is located
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
BUILD_DIR="$SCRIPT_DIR/build"

# Set up environment
export TREEWIDZARD_SEARCHSTRATEGIES="$BUILD_DIR/SearchStrategies"
export TREEWIDZARD_DPCORES="$BUILD_DIR/DPCores"

# Check if treewidzard executable exists
if [ ! -f "$BUILD_DIR/treewidzard" ]; then
    echo "Error: treewidzard executable not found at $BUILD_DIR/treewidzard"
    echo "Please build the project first with: cmake --build build"
    exit 1
fi

# Run treewidzard with all arguments passed through
"$BUILD_DIR/treewidzard" "$@"
