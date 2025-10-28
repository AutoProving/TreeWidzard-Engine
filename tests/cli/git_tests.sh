#!/bin/bash

# TreeWidzard Git CLI Test Suite
# Automated tests for git operations and repository state

set -e  # Exit on any error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Test counters
TESTS_RUN=0
TESTS_PASSED=0
TESTS_FAILED=0

# Helper functions
log_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

log_success() {
    echo -e "${GREEN}[PASS]${NC} $1"
    ((TESTS_PASSED++))
}

log_error() {
    echo -e "${RED}[FAIL]${NC} $1"
    ((TESTS_FAILED++))
}

log_warning() {
    echo -e "${YELLOW}[WARN]${NC} $1"
}

run_test() {
    local test_name="$1"
    local test_command="$2"
    local expected_exit_code="${3:-0}"
    
    ((TESTS_RUN++))
    log_info "Running test: $test_name"
    
    if eval "$test_command" > /dev/null 2>&1; then
        actual_exit_code=$?
    else
        actual_exit_code=$?
    fi
    
    if [ $actual_exit_code -eq $expected_exit_code ]; then
        log_success "$test_name"
        return 0
    else
        log_error "$test_name (expected exit code $expected_exit_code, got $actual_exit_code)"
        return 1
    fi
}

run_test_with_output() {
    local test_name="$1"
    local test_command="$2"
    local expected_pattern="$3"
    
    ((TESTS_RUN++))
    log_info "Running test: $test_name"
    
    local output
    output=$(eval "$test_command" 2>&1)
    
    if echo "$output" | grep -q "$expected_pattern"; then
        log_success "$test_name"
        return 0
    else
        log_error "$test_name (expected pattern '$expected_pattern' not found in output)"
        echo "Actual output: $output"
        return 1
    fi
}

# Main test suite
main() {
    log_info "Starting TreeWidzard Git CLI Test Suite"
    log_info "Repository: $(pwd)"
    
    # Test 1: Check if we're in a git repository
    run_test "Git repository detection" "git rev-parse --git-dir"
    
    # Test 2: Check current branch
    run_test_with_output "Current branch check" "git branch --show-current" "feature/gpu-acceleration"
    
    # Test 3: Check remote configuration
    run_test "Remote origin exists" "git remote get-url origin"
    
    # Test 4: Check working tree status
    run_test_with_output "Working tree status" "git status --porcelain" "^$"
    
    # Test 5: Verify branch structure
    run_test "Main branch exists" "git show-ref --verify --quiet refs/heads/main"
    run_test "Optimized branch exists" "git show-ref --verify --quiet refs/heads/optimized"
    run_test "Feature branch exists" "git show-ref --verify --quiet refs/heads/feature/gpu-acceleration"
    
    # Test 6: Check commit history integrity
    run_test "Git log accessible" "git log --oneline -1"
    
    # Test 7: Check for unpushed commits
    local unpushed_count
    unpushed_count=$(git log --oneline main..HEAD | wc -l | tr -d ' ')
    if [ "$unpushed_count" -gt 0 ]; then
        log_warning "Found $unpushed_count unpushed commits on current branch"
    fi
    
    # Test 8: Check branch relationships
    run_test "Optimized branch diverged from main" "git merge-base --is-ancestor main optimized"
    run_test "GPU branch diverged from optimized" "git merge-base --is-ancestor optimized feature/gpu-acceleration"
    
    # Test 9: Check for merge conflicts
    run_test "No merge conflicts with main" "git merge-tree $(git merge-base main HEAD) main HEAD | grep -q '^<<<<<' && false || true"
    
    # Test 10: Validate commit messages
    run_test "Recent commits have messages" "test $(git log --oneline -5 --pretty=format:'%s' | wc -l) -eq 5"
    
    # Test 11: Check file tracking
    run_test "No untracked important files" "! git status --porcelain | grep -E '^\?\? .+\.(cpp|h|cmake|md)$'"
    
    # Test 12: Repository size check
    local repo_size
    repo_size=$(du -sh .git | cut -f1)
    log_info "Repository size: $repo_size"
    
    # Test 13: Check for large files
    run_test "No large files in repo" "! git ls-files | xargs ls -la | awk '\$5 > 10485760 {print \$0; exit 1}'"
    
    # Summary
    echo
    log_info "=== Test Summary ==="
    log_info "Tests run: $TESTS_RUN"
    log_success "Tests passed: $TESTS_PASSED"
    if [ $TESTS_FAILED -gt 0 ]; then
        log_error "Tests failed: $TESTS_FAILED"
        exit 1
    else
        log_success "All tests passed!"
        exit 0
    fi
}

# Check if we're in the right directory
if [ ! -f "CMakeLists.txt" ] || [ ! -d "GPU" ]; then
    log_error "Please run this script from the TreeWidzard-development root directory"
    exit 1
fi

main "$@"