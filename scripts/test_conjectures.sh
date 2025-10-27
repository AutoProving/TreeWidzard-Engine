#!/bin/bash

# TreeWidzard Conjecture Testing Script
# Demonstrates how to use different conjecture formats with TreeWidzard

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"
BUILD_DIR="$PROJECT_ROOT/build"
EXAMPLES_DIR="$PROJECT_ROOT/examples/conjectures"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

log_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

log_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

log_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

log_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Function to test a conjecture file
test_conjecture() {
    local conjecture_file="$1"
    local test_name="$2"
    local width_type="${3:-pw}"
    local width_value="${4:-3}"
    local strategy="${5:-BreadthFirstSearch}"
    
    log_info "Testing: $test_name"
    log_info "Conjecture file: $conjecture_file"
    
    if [ ! -f "$conjecture_file" ]; then
        log_error "Conjecture file not found: $conjecture_file"
        return 1
    fi
    
    if [ ! -f "$BUILD_DIR/treewidzard" ]; then
        log_error "TreeWidzard executable not found. Run 'cmake --build Build' first."
        return 1
    fi
    
    echo "=== Conjecture Content ==="
    cat "$conjecture_file"
    echo "=========================="
    
    # Test with automated theorem proving mode
    log_info "Running ATP mode with $strategy..."
    cd "$PROJECT_ROOT"
    
    timeout 60s "$BUILD_DIR/treewidzard" \
        -atp pw=3 -pl \
        "$SEARCH_STRATEGY" \
        "$conjecture_file" || {
        local exit_code=$?
        if [ $exit_code -eq 124 ]; then
            log_warning "Test timed out after 60 seconds"
        else
            log_warning "Test exited with code $exit_code"
        fi
    }
    
    echo ""
    log_success "Completed: $test_name"
    echo "================================================"
    echo ""
}

# Function to test with model checking mode (requires graph files)
test_model_checking() {
    local conjecture_file="$1"
    local graph_file="$2"
    local decomposition_file="$3"
    local test_name="$4"
    
    log_info "Testing Model Checking: $test_name"
    
    if [ ! -f "$graph_file" ] || [ ! -f "$decomposition_file" ]; then
        log_warning "Graph or decomposition file not found for model checking test"
        return 1
    fi
    
    cd "$PROJECT_ROOT"
    
    timeout 60s "$BUILD_DIR/treewidzard" \
        -modelcheck PACE \
        "$conjecture_file" \
        "$graph_file" \
        "$decomposition_file" || {
        log_warning "Model checking test failed or timed out"
    }
    
    echo ""
}

main() {
    log_info "TreeWidzard Conjecture Testing Suite"
    log_info "====================================="
    echo ""
    
    # Check if TreeWidzard is built
    if [ ! -f "$BUILD_DIR/treewidzard" ]; then
        log_error "TreeWidzard not found. Building..."
        cd "$PROJECT_ROOT"
        cmake -BBuild
        cmake --build Build
    fi
    
    case "${1:-all}" in
        "simple")
            log_info "Running simple conjecture tests..."
            test_conjecture "$EXAMPLES_DIR/simple_3_coloring.txt" "Simple 3-Coloring Test"
            ;;
            
        "brooks")
            log_info "Running Brooks' theorem test..."
            test_conjecture "$EXAMPLES_DIR/brooks_theorem.txt" "Brooks' Theorem Test" "pw" "4"
            ;;
            
        "independence")
            log_info "Running independence number tests..."
            test_conjecture "$EXAMPLES_DIR/independence_bound.txt" "Independence Bound Test"
            test_conjecture "$EXAMPLES_DIR/clique_independence.txt" "Clique-Independence Relationship"
            ;;
            
        "advanced")
            log_info "Running advanced conjecture tests..."
            test_conjecture "$EXAMPLES_DIR/ramsey_property.txt" "Ramsey-type Property" "tw" "4"
            test_conjecture "$EXAMPLES_DIR/degree_coloring_bounds.txt" "Degree-Coloring Bounds" "pw" "5"
            test_conjecture "$EXAMPLES_DIR/small_graph_properties.txt" "Small Graph Properties"
            ;;
            
        "multigraph")
            log_info "Running multigraph tests..."
            test_conjecture "$EXAMPLES_DIR/multigraph_properties.txt" "Multigraph Properties"
            ;;
            
        "parallel")
            log_info "Running parallel search strategy tests..."
            test_conjecture "$EXAMPLES_DIR/simple_3_coloring.txt" "Parallel BFS Test" "pw" "3" "ParallelBreadthFirstSearch"
            test_conjecture "$EXAMPLES_DIR/brooks_theorem.txt" "Parallel Isomorphism BFS" "tw" "4" "ParallelIsomorphismBreadthFirstSearch"
            ;;
            
        "model_check")
            log_info "Running model checking tests..."
            # Test with existing system-paper examples
            if [ -f "$PROJECT_ROOT/system-paper/model-checking-option-1/diamond.gr" ]; then
                test_model_checking \
                    "$PROJECT_ROOT/system-paper/model-checking-option-1/three_colorable.txt" \
                    "$PROJECT_ROOT/system-paper/model-checking-option-1/diamond.gr" \
                    "$PROJECT_ROOT/system-paper/model-checking-option-1/diamond.td" \
                    "Diamond Graph 3-Coloring"
            fi
            ;;
            
        "all")
            log_info "Running complete test suite..."
            
            # Basic tests
            test_conjecture "$EXAMPLES_DIR/simple_3_coloring.txt" "Simple 3-Coloring Test"
            test_conjecture "$EXAMPLES_DIR/brooks_theorem.txt" "Brooks' Theorem Test" "pw" "4"
            
            # Independence and clique tests
            test_conjecture "$EXAMPLES_DIR/independence_bound.txt" "Independence Bound Test"
            test_conjecture "$EXAMPLES_DIR/clique_independence.txt" "Clique-Independence Relationship"
            
            # Advanced properties
            test_conjecture "$EXAMPLES_DIR/ramsey_property.txt" "Ramsey-type Property" "tw" "4"
            test_conjecture "$EXAMPLES_DIR/small_graph_properties.txt" "Small Graph Properties"
            
            # Parallel strategies
            test_conjecture "$EXAMPLES_DIR/simple_3_coloring.txt" "Parallel BFS Test" "pw" "3" "ParallelBreadthFirstSearch"
            
            log_success "All tests completed!"
            ;;
            
        "help"|*)
            cat << EOF
TreeWidzard Conjecture Testing Script

Usage: $0 <test_type>

Test Types:
  simple        Test basic 3-coloring conjecture
  brooks        Test Brooks' theorem conjecture
  independence  Test independence number conjectures
  advanced      Test advanced multi-variable conjectures
  multigraph    Test multigraph detection properties
  parallel      Test parallel search strategies
  model_check   Test model checking mode (requires graph files)
  all           Run all available tests
  help          Show this help message

Examples:
  $0 simple                    # Run simple coloring test
  $0 all                       # Run complete test suite
  $0 parallel                  # Test parallel algorithms

Note: Tests use small width bounds and timeouts for demonstration.
For real research, use appropriate width bounds and remove timeouts.
EOF
            ;;
    esac
}

# Create the conjecture examples directory if it doesn't exist
mkdir -p "$EXAMPLES_DIR"

# Run main function with all arguments
main "$@"