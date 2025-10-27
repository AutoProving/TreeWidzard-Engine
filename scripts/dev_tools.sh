#!/bin/bash

# TreeWidzard Development Tools
# Provides utilities for development, testing, and deployment

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"
BUILD_DIR="$PROJECT_ROOT/Build"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Logging functions
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

# Build functions
build_clean() {
    log_info "Cleaning build directory..."
    rm -rf "$BUILD_DIR"
    log_success "Build directory cleaned"
}

build_configure() {
    log_info "Configuring build..."
    mkdir -p "$BUILD_DIR"
    cd "$BUILD_DIR"
    cmake .. "${@}"
    log_success "Build configured"
}

build_compile() {
    log_info "Compiling project..."
    cd "$BUILD_DIR"
    make -j$(nproc) "${@}"
    log_success "Project compiled"
}

build_all() {
    build_clean
    build_configure "${@}"
    build_compile
}

# Testing functions
run_unit_tests() {
    log_info "Running unit tests..."
    if [ -f "$BUILD_DIR/run_tests" ]; then
        cd "$BUILD_DIR"
        ./run_tests
        log_success "Unit tests completed"
    else
        log_warning "Unit tests not built. Run 'build_with_tests' first."
    fi
}

run_integration_tests() {
    log_info "Running integration tests..."
    cd "$PROJECT_ROOT"
    
    # Test basic functionality
    test_graph_coloring
    test_independence_number
    test_parallel_execution
    
    log_success "Integration tests completed"
}

test_graph_coloring() {
    log_info "Testing graph coloring..."
    
    if [ -f "$BUILD_DIR/treewidzard" ]; then
        cd "$PROJECT_ROOT"
        timeout 30s "$BUILD_DIR/treewidzard" -modelcheck PACE \
            system-paper/model-checking-option-1/three_colorable.txt \
            system-paper/model-checking-option-1/diamond.gr \
            system-paper/model-checking-option-1/diamond.td
        
        log_success "Graph coloring test passed"
    else
        log_error "TreeWidzard executable not found"
        return 1
    fi
}

test_independence_number() {
    log_info "Testing independence number computation..."
    
    if [ -f "$BUILD_DIR/treewidzard" ]; then
        cd "$PROJECT_ROOT"
        timeout 30s "$BUILD_DIR/treewidzard" -modelcheck ITD \
            system-paper/invariant-computation/independence_number.txt \
            system-paper/invariant-computation/diamondITD.txt
        
        log_success "Independence number test passed"
    else
        log_error "TreeWidzard executable not found"
        return 1
    fi
}

test_parallel_execution() {
    log_info "Testing parallel execution..."
    
    if [ -f "$BUILD_DIR/treewidzard" ]; then
        cd "$PROJECT_ROOT"
        timeout 60s "$BUILD_DIR/treewidzard" -atp pw=3 -pl -nthreads 4 \
            ParallelBreadthFirstSearch \
            system-paper/ATP/four_colorable.txt
        
        log_success "Parallel execution test passed"
    else
        log_error "TreeWidzard executable not found"
        return 1
    fi
}

# Benchmarking functions
run_benchmarks() {
    log_info "Running performance benchmarks..."
    
    if [ -f "$BUILD_DIR/benchmarks" ]; then
        cd "$BUILD_DIR"
        ./benchmarks --benchmark_format=json --benchmark_out=benchmark_results.json
        log_success "Benchmarks completed. Results in benchmark_results.json"
    else
        log_warning "Benchmarks not built. Install Google Benchmark and rebuild."
    fi
}

# Code quality functions
run_static_analysis() {
    log_info "Running static analysis..."
    
    # Check if clang-tidy is available
    if command -v clang-tidy &> /dev/null; then
        find "$PROJECT_ROOT" -name "*.cpp" -o -name "*.h" | \
        grep -v "/Build/" | \
        xargs clang-tidy -checks='-*,readability-*,performance-*,modernize-*'
        
        log_success "Static analysis completed"
    else
        log_warning "clang-tidy not found. Install LLVM tools for static analysis."
    fi
}

check_code_format() {
    log_info "Checking code formatting..."
    
    if command -v clang-format &> /dev/null; then
        find "$PROJECT_ROOT" -name "*.cpp" -o -name "*.h" | \
        grep -v "/Build/" | \
        xargs clang-format --dry-run --Werror
        
        log_success "Code formatting check completed"
    else
        log_warning "clang-format not found. Install LLVM tools for formatting checks."
    fi
}

format_code() {
    log_info "Formatting code..."
    
    if command -v clang-format &> /dev/null; then
        find "$PROJECT_ROOT" -name "*.cpp" -o -name "*.h" | \
        grep -v "/Build/" | \
        xargs clang-format -i
        
        log_success "Code formatted"
    else
        log_warning "clang-format not found. Install LLVM tools for formatting."
    fi
}

# Documentation functions
generate_docs() {
    log_info "Generating documentation..."
    
    if command -v doxygen &> /dev/null; then
        cd "$PROJECT_ROOT"
        doxygen docs/Doxyfile
        log_success "Documentation generated in docs/html/"
    else
        log_warning "Doxygen not found. Install Doxygen for documentation generation."
    fi
}

# Package functions
create_package() {
    local version="${1:-dev}"
    log_info "Creating package for version $version..."
    
    cd "$PROJECT_ROOT"
    
    # Create package directory
    local package_dir="treewidzard-$version"
    mkdir -p "$package_dir"
    
    # Copy binary and essential files
    cp "$BUILD_DIR/treewidzard" "$package_dir/"
    cp README.md LICENSE "$package_dir/"
    cp -r system-paper "$package_dir/examples"
    cp -r docs "$package_dir/"
    
    # Create archive
    tar -czf "treewidzard-$version.tar.gz" "$package_dir"
    rm -rf "$package_dir"
    
    log_success "Package created: treewidzard-$version.tar.gz"
}

# Performance testing functions
run_performance_profile() {
    local test_case="${1:-system-paper/ATP/four_colorable.txt}"
    log_info "Running performance profile on $test_case..."
    
    if [ -f "$BUILD_DIR/treewidzard" ]; then
        cd "$PROJECT_ROOT"
        
        # Enable performance profiling
        export TREEWIDZARD_PROFILE=1
        export TREEWIDZARD_DETAILED_PROFILE=1
        
        # Run with profiling enabled
        timeout 300s "$BUILD_DIR/treewidzard" -atp pw=3 -pl -nthreads 4 \
            ParallelBreadthFirstSearch "$test_case"
        
        log_success "Performance profiling completed. Check performance_report.txt"
    else
        log_error "TreeWidzard executable not found"
        return 1
    fi
}

benchmark_performance_optimizations() {
    log_info "Benchmarking performance optimizations..."
    
    if [ -f "$BUILD_DIR/benchmarks" ]; then
        cd "$BUILD_DIR"
        
        # Run memory pool benchmarks
        ./benchmarks --benchmark_filter="MemoryPool.*" --benchmark_format=console
        
        # Run cache benchmarks
        ./benchmarks --benchmark_filter="Cache.*" --benchmark_format=console
        
        # Run parallel algorithm benchmarks
        ./benchmarks --benchmark_filter="Parallel.*" --benchmark_format=console
        
        log_success "Performance benchmarks completed"
    else
        log_warning "Benchmarks not built. Run 'build' with benchmark support first."
    fi
}

# Memory usage monitoring
monitor_memory_usage() {
    local pid="$1"
    local output_file="${2:-memory_usage.log}"
    
    log_info "Monitoring memory usage for process $pid..."
    
    echo "timestamp,rss_mb,vms_mb,cpu_percent" > "$output_file"
    
    while kill -0 "$pid" 2>/dev/null; do
        local timestamp=$(date '+%Y-%m-%d %H:%M:%S')
        local memory_info=$(ps -p "$pid" -o rss=,vsz=,pcpu= | tr -s ' ')
        
        if [ -n "$memory_info" ]; then
            local rss=$(echo "$memory_info" | awk '{print $1/1024}')  # KB to MB
            local vms=$(echo "$memory_info" | awk '{print $2/1024}')  # KB to MB
            local cpu=$(echo "$memory_info" | awk '{print $3}')
            
            echo "$timestamp,$rss,$vms,$cpu" >> "$output_file"
        fi
        
        sleep 1
    done
    
    log_success "Memory monitoring completed. Data saved to $output_file"
}

# CPU profiling with perf (if available)
run_cpu_profile() {
    local command="$1"
    local output_file="${2:-cpu_profile.data}"
    
    if command -v perf &> /dev/null; then
        log_info "Running CPU profiling with perf..."
        perf record -g -o "$output_file" $command
        
        log_info "Generating CPU profile report..."
        perf report -i "$output_file" --stdio > "${output_file%.data}.txt"
        
        log_success "CPU profiling completed. Report saved to ${output_file%.data}.txt"
    else
        log_warning "perf not available. Install linux-tools for CPU profiling."
    fi
}

# NUMA topology analysis
check_numa_topology() {
    log_info "Checking NUMA topology..."
    
    if command -v numactl &> /dev/null; then
        echo "NUMA nodes available:"
        numactl --hardware
        
        echo -e "\nCurrent NUMA policy:"
        numactl --show
        
        echo -e "\nNUMA memory usage:"
        numastat
        
        log_success "NUMA topology analysis completed"
    else
        log_warning "numactl not available. Install numactl for NUMA analysis."
    fi
}
setup_development_environment() {
    log_info "Setting up development environment..."
    
    # Install dependencies based on OS
    if [[ "$OSTYPE" == "linux-gnu"* ]]; then
        setup_linux_dependencies
    elif [[ "$OSTYPE" == "darwin"* ]]; then
        setup_macos_dependencies
    else
        log_warning "Unsupported OS. Manual dependency installation required."
    fi
    
    # Setup Git hooks
    setup_git_hooks
    
    log_success "Development environment setup completed"
}

setup_linux_dependencies() {
    log_info "Installing Linux dependencies..."
    
    # Detect package manager
    if command -v apt-get &> /dev/null; then
        sudo apt-get update
        sudo apt-get install -y cmake g++ libgtest-dev libbenchmark-dev \
                               clang-tidy clang-format doxygen
    elif command -v yum &> /dev/null; then
        sudo yum install -y cmake gcc-c++ gtest-devel benchmark-devel \
                           clang-tools-extra doxygen
    else
        log_warning "Unsupported package manager. Install dependencies manually."
    fi
}

setup_macos_dependencies() {
    log_info "Installing macOS dependencies..."
    
    if command -v brew &> /dev/null; then
        brew install cmake googletest google-benchmark llvm doxygen
    else
        log_warning "Homebrew not found. Install dependencies manually."
    fi
}

setup_git_hooks() {
    log_info "Setting up Git hooks..."
    
    # Pre-commit hook for code formatting
    cat > "$PROJECT_ROOT/.git/hooks/pre-commit" << 'EOF'
#!/bin/bash
echo "Running pre-commit checks..."

# Check code formatting
if ! ./scripts/dev_tools.sh check_code_format; then
    echo "Code formatting check failed. Run './scripts/dev_tools.sh format_code' to fix."
    exit 1
fi

# Run quick tests
if ! ./scripts/dev_tools.sh run_unit_tests; then
    echo "Unit tests failed."
    exit 1
fi

echo "Pre-commit checks passed."
EOF

    chmod +x "$PROJECT_ROOT/.git/hooks/pre-commit"
    log_success "Git hooks installed"
}

# Main command dispatcher
main() {
    case "${1:-help}" in
        # Build commands
        "build")
            build_all "${@:2}"
            ;;
        "build_clean")
            build_clean
            ;;
        "build_configure")
            build_configure "${@:2}"
            ;;
        "build_compile")
            build_compile "${@:2}"
            ;;
        
        # Test commands
        "test")
            run_unit_tests
            run_integration_tests
            ;;
        "test_unit")
            run_unit_tests
            ;;
        "test_integration")
            run_integration_tests
            ;;
        "benchmark")
            run_benchmarks
            ;;
        
        # Code quality commands
        "lint")
            run_static_analysis
            ;;
        "format")
            format_code
            ;;
        "check_format")
            check_code_format
            ;;
        
        # Documentation commands
        "docs")
            generate_docs
            ;;
        
        # Package commands
        "package")
            create_package "${2:-dev}"
            ;;
        
        # Setup commands
        "setup")
            setup_development_environment
            ;;
        
        # Performance commands
        "profile")
            run_performance_profile "${@:2}"
            ;;
        "benchmark_performance")
            benchmark_performance_optimizations
            ;;
        
        # Help
        "help"|*)
            cat << EOF
TreeWidzard Development Tools

Usage: $0 <command> [options]

Build Commands:
  build                 Clean, configure, and compile the project
  build_clean          Clean the build directory
  build_configure      Configure the build system
  build_compile        Compile the project

Test Commands:
  test                 Run all tests
  test_unit           Run unit tests only
  test_integration    Run integration tests only
  benchmark           Run performance benchmarks

Code Quality Commands:
  lint                Run static analysis
  format              Format code using clang-format
  check_format        Check code formatting

Documentation Commands:
  docs                Generate documentation

Package Commands:
  package [version]   Create distribution package

Performance Commands:
  profile [test_case]     Run performance profiling on a test case
  benchmark_performance   Run performance optimization benchmarks

Setup Commands:
  setup               Setup development environment

Other Commands:
  help                Show this help message

Examples:
  $0 build                    # Build the entire project
  $0 test                     # Run all tests
  $0 format                   # Format all code
  $0 package v1.0.0          # Create v1.0.0 package
EOF
            ;;
    esac
}

# Run main function with all arguments
main "$@"