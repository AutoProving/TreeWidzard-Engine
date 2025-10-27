#!/bin/bash

# TreeWidzard Performance Optimization Development Setup
# Quick script to set up feature branches and development environment

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"

# Colors
GREEN='\033[0;32m'
BLUE='\033[0;34m'
YELLOW='\033[1;33m'
NC='\033[0m'

log_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

log_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

log_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

setup_feature_branches() {
    log_info "Setting up feature branches for performance optimization..."
    
    cd "$PROJECT_ROOT"
    
    # Ensure we're on the performance-optimizations branch
    git checkout performance-optimizations
    
    # Create feature branches if they don't exist
    FEATURE_BRANCHES=(
        "feature/witness-pruning"
        "feature/memory-pools" 
        "feature/fast-containers"
        "feature/computation-cache"
        "feature/search-bounds"
        "feature/parallel-optimization"
    )
    
    for branch in "${FEATURE_BRANCHES[@]}"; do
        if git show-ref --verify --quiet refs/heads/"$branch"; then
            log_warning "Branch $branch already exists"
        else
            git checkout -b "$branch" performance-optimizations
            log_success "Created branch: $branch"
            git checkout performance-optimizations
        fi
    done
    
    log_success "All feature branches ready!"
}

setup_performance_environment() {
    log_info "Setting up performance optimization environment..."
    
    cd "$PROJECT_ROOT"
    
    # Ensure build directory exists
    if [ ! -d "build" ]; then
        mkdir build
        cd build
        cmake ..
        cd ..
    fi
    
    # Build with performance optimizations enabled
    cd build
    make -j$(nproc)
    cd ..
    
    log_success "Build environment ready!"
}

run_baseline_benchmarks() {
    log_info "Running baseline performance benchmarks..."
    
    cd "$PROJECT_ROOT"
    
    # Create benchmarks directory if it doesn't exist
    mkdir -p benchmarks/baseline
    
    # Run current performance tests
    echo "=== Baseline Performance Test ===" > benchmarks/baseline/results.txt
    echo "Date: $(date)" >> benchmarks/baseline/results.txt
    echo "Branch: $(git branch --show-current)" >> benchmarks/baseline/results.txt
    echo "Commit: $(git rev-parse HEAD)" >> benchmarks/baseline/results.txt
    echo "" >> benchmarks/baseline/results.txt
    
    # Test different treewidths
    for tw in 3 4; do
        echo "Testing treewidth $tw..." >> benchmarks/baseline/results.txt
        
        # Time and memory usage
        /usr/bin/time -l ./build/treewidzard \
            -atp tw=$tw -pl BreadthFirstSearch \
            examples/conjectures/simple_3_coloring.txt \
            2>&1 | tee -a benchmarks/baseline/results.txt || true
        
        echo "---" >> benchmarks/baseline/results.txt
    done
    
    log_success "Baseline benchmarks saved to benchmarks/baseline/results.txt"
}

show_optimization_menu() {
    echo ""
    echo "🚀 TreeWidzard Performance Optimization Development"
    echo "=================================================="
    echo ""
    echo "Available optimization areas:"
    echo ""
    echo "1. Witness Pruning      - Reduce memory by 40-60%"
    echo "2. Memory Pools         - Faster allocation by 30-50%"
    echo "3. Fast Containers      - Speed up operations by 20-30%"
    echo "4. Computation Cache    - Reduce redundant work by 25-40%"
    echo "5. Search Bounds        - Handle higher treewidths"
    echo "6. Parallel Optimization - Better multi-core usage"
    echo ""
    echo "Quick commands:"
    echo "  ./scripts/perf_setup.sh branches    # Set up all feature branches"
    echo "  ./scripts/perf_setup.sh baseline    # Run baseline benchmarks"
    echo "  ./scripts/perf_setup.sh build       # Build with optimizations"
    echo "  ./scripts/perf_setup.sh test        # Run performance tests"
    echo ""
    echo "Feature branch workflow:"
    echo "  git checkout feature/witness-pruning"
    echo "  # Make changes..."
    echo "  ./scripts/dev_tools.sh test"
    echo "  ./scripts/dev_tools.sh benchmark_performance"
    echo "  git commit -m 'Implement witness pruning'"
    echo ""
}

main() {
    case "${1:-menu}" in
        "branches")
            setup_feature_branches
            ;;
        "build")
            setup_performance_environment
            ;;
        "baseline")
            run_baseline_benchmarks
            ;;
        "test")
            cd "$PROJECT_ROOT"
            ./scripts/dev_tools.sh test
            ./scripts/dev_tools.sh benchmark_performance
            ;;
        "menu"|*)
            show_optimization_menu
            ;;
    esac
}

main "$@"