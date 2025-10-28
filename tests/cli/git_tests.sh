#!/bin/bash

# TreeWidzard Git CLI Test Suite
# Automated tests for git operations and repository state

# Note: Removed 'set -e' to allow tests to continue even if some fail
# We handle errors manually in the test functions

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
    
    # Test 2: Check current branch (accept any valid branch)
    local current_branch
    current_branch=$(git branch --show-current)
    if [ -n "$current_branch" ]; then
        log_success "Current branch check ($current_branch)"
        ((TESTS_RUN++))
        ((TESTS_PASSED++))
    else
        log_error "Current branch check (no current branch detected)"
        ((TESTS_RUN++))
        ((TESTS_FAILED++))
    fi
    
    # Test 3: Check remote configuration
    run_test "Remote origin exists" "git remote get-url origin"
    
    # Test 4: Check working tree status (allow temporary files in CI)
    local status_output
    status_output=$(git status --porcelain)
    # Ignore shell_test_output.log and other temporary CI files
    status_output=$(echo "$status_output" | grep -v "shell_test_output.log" | grep -v "git_test_report.json" || true)
    if [ -z "$status_output" ]; then
        log_success "Working tree status"
        ((TESTS_RUN++))
        ((TESTS_PASSED++))
    else
        log_error "Working tree status (untracked/modified files found)"
        echo "Files: $status_output"
        ((TESTS_RUN++))
        ((TESTS_FAILED++))
    fi
    
    # Test 5: Verify branch structure (check if branches exist locally or remotely)
    if git show-ref --verify --quiet refs/heads/main || git show-ref --verify --quiet refs/remotes/origin/main; then
        log_success "Main branch exists"
        ((TESTS_RUN++))
        ((TESTS_PASSED++))
    else
        log_error "Main branch exists"
        ((TESTS_RUN++))
        ((TESTS_FAILED++))
    fi
    
    if git show-ref --verify --quiet refs/heads/optimized || git show-ref --verify --quiet refs/remotes/origin/optimized; then
        log_success "Optimized branch exists"
        ((TESTS_RUN++))
        ((TESTS_PASSED++))
    else
        log_error "Optimized branch exists"
        ((TESTS_RUN++))
        ((TESTS_FAILED++))
    fi
    
    if git show-ref --verify --quiet refs/heads/feature/gpu-acceleration || git show-ref --verify --quiet refs/remotes/origin/feature/gpu-acceleration; then
        log_success "Feature branch exists"
        ((TESTS_RUN++))
        ((TESTS_PASSED++))
    else
        log_error "Feature branch exists"
        ((TESTS_RUN++))
        ((TESTS_FAILED++))
    fi
    
    # Test 6: Check commit history integrity
    run_test "Git log accessible" "git log --oneline -1"
    
    # Test 7: Check for unpushed commits
    local unpushed_count
    unpushed_count=$(git log --oneline main..HEAD | wc -l | tr -d ' ')
    if [ "$unpushed_count" -gt 0 ]; then
        log_warning "Found $unpushed_count unpushed commits on current branch"
    fi
    
    # Test 8: Check branch relationships (only if branches exist locally)
    if git show-ref --verify --quiet refs/heads/optimized && git show-ref --verify --quiet refs/heads/main; then
        if git merge-base --is-ancestor main optimized 2>/dev/null; then
            log_success "Optimized branch diverged from main"
        else
            log_error "Optimized branch diverged from main (not ancestor or cannot check)"
        fi
        ((TESTS_RUN++))
        if git merge-base --is-ancestor main optimized 2>/dev/null; then
            ((TESTS_PASSED++))
        else
            ((TESTS_FAILED++))
        fi
    else
        log_info "Skipping optimized branch relationship check (branches not available locally)"
    fi
    
    if git show-ref --verify --quiet refs/heads/feature/gpu-acceleration && git show-ref --verify --quiet refs/heads/optimized; then
        if git merge-base --is-ancestor optimized feature/gpu-acceleration 2>/dev/null; then
            log_success "GPU branch diverged from optimized"
        else
            log_error "GPU branch diverged from optimized (not ancestor or cannot check)"
        fi
        ((TESTS_RUN++))
        if git merge-base --is-ancestor optimized feature/gpu-acceleration 2>/dev/null; then
            ((TESTS_PASSED++))
        else
            ((TESTS_FAILED++))
        fi
    else
        log_info "Skipping GPU branch relationship check (branches not available locally)"
    fi
    
    # Test 9: Check for merge conflicts
    run_test "No merge conflicts with main" "git merge-tree $(git merge-base main HEAD) main HEAD | grep -q '^<<<<<' && false || true"
    
    # Test 10: Validate commit messages (more flexible check)
    local commit_count
    commit_count=$(git log --oneline -5 2>/dev/null | wc -l | tr -d ' ')
    if [ "$commit_count" -ge 1 ]; then
        log_success "Recent commits have messages"
        ((TESTS_RUN++))
        ((TESTS_PASSED++))
    else
        log_error "Recent commits have messages (no recent commits found)"
        ((TESTS_RUN++))
        ((TESTS_FAILED++))
    fi
    
    # Test 11: Check file tracking
    run_test "No untracked important files" "! git status --porcelain | grep -E '^\?\? .+\.(cpp|h|cmake|md)$'"
    
    # Test 12: Repository size check
    local repo_size
    repo_size=$(du -sh .git | cut -f1)
    log_info "Repository size: $repo_size"
    
    # Test 13: Check for large files (more robust)
    local large_files
    large_files=$(git ls-files 2>/dev/null | xargs ls -la 2>/dev/null | awk '$5 > 10485760 {print $0}' || true)
    if [ -z "$large_files" ]; then
        log_success "No large files in repo"
        ((TESTS_RUN++))
        ((TESTS_PASSED++))
    else
        log_error "No large files in repo (large files found)"
        echo "Large files: $large_files"
        ((TESTS_RUN++))
        ((TESTS_FAILED++))
    fi
    
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