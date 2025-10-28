#!/bin/bash

# TreeWidzard Git CLI Test Runner
# Wrapper script to run all git tests

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/../.." && pwd)"

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

log_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

log_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

log_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

log_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

show_help() {
    cat << EOF
TreeWidzard Git CLI Test Runner

Usage: $0 [OPTIONS]

OPTIONS:
    -h, --help          Show this help message
    -v, --verbose       Run tests in verbose mode
    -q, --quick         Run only critical tests
    -r, --report        Generate detailed report
    -f, --format FORMAT Output format (text|json|html)
    --shell             Run shell-based tests only
    --python            Run Python-based tests only
    --all               Run all available tests (default)

EXAMPLES:
    $0                  # Run all tests
    $0 --quick          # Run critical tests only
    $0 --python -r      # Run Python tests and generate report
    $0 --verbose        # Run tests with detailed output
EOF
}

run_shell_tests() {
    log_info "Running shell-based git tests..."
    cd "$PROJECT_ROOT"
    
    if [ -x "$SCRIPT_DIR/git_tests.sh" ]; then
        "$SCRIPT_DIR/git_tests.sh"
        return $?
    else
        log_error "Shell test script not found or not executable"
        return 1
    fi
}

run_python_tests() {
    log_info "Running Python-based git tests..."
    cd "$PROJECT_ROOT"
    
    # Check if Python 3 is available
    if ! command -v python3 &> /dev/null; then
        log_error "Python 3 is required but not installed"
        return 1
    fi
    
    if [ -f "$SCRIPT_DIR/git_tests.py" ]; then
        python3 "$SCRIPT_DIR/git_tests.py"
        return $?
    else
        log_error "Python test script not found"
        return 1
    fi
}

check_prerequisites() {
    log_info "Checking prerequisites..."
    
    # Check if we're in a git repository
    if ! git rev-parse --git-dir &> /dev/null; then
        log_error "Not in a git repository"
        return 1
    fi
    
    # Check if we're in the correct project
    if [ ! -f "$PROJECT_ROOT/CMakeLists.txt" ] || [ ! -d "$PROJECT_ROOT/GPU" ]; then
        log_error "Not in TreeWidzard project directory"
        return 1
    fi
    
    # Make shell script executable
    chmod +x "$SCRIPT_DIR/git_tests.sh" 2>/dev/null || true
    
    log_success "Prerequisites check passed"
    return 0
}

main() {
    local run_shell=false
    local run_python=false
    local run_all=true
    local verbose=false
    local quick=false
    local generate_report=false
    local output_format="text"
    
    # Parse command line arguments
    while [[ $# -gt 0 ]]; do
        case $1 in
            -h|--help)
                show_help
                exit 0
                ;;
            -v|--verbose)
                verbose=true
                shift
                ;;
            -q|--quick)
                quick=true
                shift
                ;;
            -r|--report)
                generate_report=true
                shift
                ;;
            -f|--format)
                output_format="$2"
                shift 2
                ;;
            --shell)
                run_shell=true
                run_all=false
                shift
                ;;
            --python)
                run_python=true
                run_all=false
                shift
                ;;
            --all)
                run_all=true
                shift
                ;;
            *)
                log_error "Unknown option: $1"
                show_help
                exit 1
                ;;
        esac
    done
    
    # Check prerequisites
    if ! check_prerequisites; then
        exit 1
    fi
    
    log_info "Starting TreeWidzard Git CLI Tests"
    log_info "Project root: $PROJECT_ROOT"
    log_info "Test directory: $SCRIPT_DIR"
    
    local overall_success=true
    
    # Run tests based on options
    if [ "$run_all" = true ] || [ "$run_shell" = true ]; then
        if ! run_shell_tests; then
            overall_success=false
        fi
        echo
    fi
    
    if [ "$run_all" = true ] || [ "$run_python" = true ]; then
        if ! run_python_tests; then
            overall_success=false
        fi
        echo
    fi
    
    # Show final results
    if [ "$overall_success" = true ]; then
        log_success "All git tests completed successfully!"
        
        # Show report location if generated
        if [ "$generate_report" = true ] && [ -f "$PROJECT_ROOT/tests/cli/git_test_report.json" ]; then
            log_info "Detailed report available at: tests/cli/git_test_report.json"
        fi
        
        exit 0
    else
        log_error "Some tests failed. Please check the output above."
        exit 1
    fi
}

# Change to project root directory
cd "$PROJECT_ROOT"

main "$@"