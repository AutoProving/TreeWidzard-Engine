#!/bin/bash

# TreeWidzard Multi-Branch Git CLI Test Runner
# Run automated tests across all branches to ensure consistency

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
PURPLE='\033[0;35m'
CYAN='\033[0;36m'
NC='\033[0m'

# Test results tracking
TOTAL_BRANCHES=0
PASSED_BRANCHES=0
FAILED_BRANCHES=()
BRANCH_RESULTS=()

log_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

log_success() {
    echo -e "${GREEN}[PASS]${NC} $1"
}

log_error() {
    echo -e "${RED}[FAIL]${NC} $1"
}

log_warning() {
    echo -e "${YELLOW}[WARN]${NC} $1"
}

log_branch() {
    echo -e "${PURPLE}[BRANCH]${NC} $1"
}

show_help() {
    cat << EOF
TreeWidzard Multi-Branch Git CLI Test Runner

DESCRIPTION:
    Run comprehensive git tests across all branches to ensure repository
    consistency and health across different development streams.

USAGE: 
    $0 [OPTIONS]

OPTIONS:
    -h, --help              Show this help message
    -b, --branches BRANCHES Comma-separated list of branches to test
    -e, --exclude BRANCHES  Comma-separated list of branches to exclude
    -q, --quick             Run only critical tests
    -r, --report            Generate detailed reports for each branch
    -c, --compare           Compare test results between branches
    -s, --summary           Show summary table at the end
    --shell-only            Run only shell-based tests
    --python-only           Run only Python-based tests
    --no-checkout           Don't switch branches (test current branch only)

EXAMPLES:
    $0                                          # Test all branches
    $0 -b main,optimized                        # Test only main and optimized
    $0 -e feature/gpu-acceleration              # Test all except GPU feature
    $0 --quick --summary                        # Quick tests with summary
    $0 --compare --report                       # Full comparison with reports
    $0 --no-checkout                            # Test current branch only

BRANCH-SPECIFIC BEHAVIOR:
    - main: Core repository tests, basic structure validation
    - optimized: Performance-focused tests, optimization validation
    - feature/*: Feature-specific tests, integration validation
    - All branches: Security, git health, and consistency checks
EOF
}

get_current_branch() {
    git branch --show-current
}

get_all_branches() {
    git branch --format='%(refname:short)' | grep -v '^remotes/' | sort
}

backup_current_state() {
    local current_branch=$(get_current_branch)
    local has_changes=$(git status --porcelain | wc -l | tr -d ' ')
    
    if [ "$has_changes" -gt 0 ]; then
        log_warning "Working tree has uncommitted changes. Creating backup stash..."
        git stash push -m "Multi-branch test backup $(date)"
        echo "stashed"
    else
        echo "clean"
    fi
}

restore_state() {
    local original_branch="$1"
    local stash_state="$2"
    
    log_info "Restoring original state..."
    git checkout "$original_branch" 2>/dev/null
    
    if [ "$stash_state" = "stashed" ]; then
        log_info "Restoring stashed changes..."
        git stash pop
    fi
}

check_branch_test_requirements() {
    local branch="$1"
    local missing_files=()
    
    # Check if test files exist on this branch
    if [ ! -f "tests/cli/run_tests.sh" ]; then
        missing_files+=("tests/cli/run_tests.sh")
    fi
    
    if [ ! -f "tests/cli/git_tests.sh" ]; then
        missing_files+=("tests/cli/git_tests.sh")
    fi
    
    if [ ${#missing_files[@]} -gt 0 ]; then
        log_warning "Branch '$branch' missing test files: ${missing_files[*]}"
        return 1
    fi
    
    return 0
}

run_branch_tests() {
    local branch="$1"
    local test_type="$2"
    local quick_mode="$3"
    local generate_report="$4"
    
    log_branch "Testing branch: $branch"
    
    # Check if tests exist on this branch
    if ! check_branch_test_requirements "$branch"; then
        log_warning "Skipping branch '$branch' - test infrastructure not available"
        return 2
    fi
    
    # Build test command
    local test_cmd="./tests/cli/run_tests.sh"
    
    case "$test_type" in
        "shell")
            test_cmd="$test_cmd --shell"
            ;;
        "python")
            test_cmd="$test_cmd --python"
            ;;
        "all")
            test_cmd="$test_cmd --all"
            ;;
    esac
    
    if [ "$quick_mode" = "true" ]; then
        test_cmd="$test_cmd --quick"
    fi
    
    if [ "$generate_report" = "true" ]; then
        test_cmd="$test_cmd --report"
        # Copy report with branch name
        local report_file="tests/cli/git_test_report_${branch//\//_}.json"
    fi
    
    # Run tests with timeout (use gtimeout if available, otherwise run without timeout)
    local start_time=$(date +%s)
    local timeout_cmd=""
    if command -v gtimeout >/dev/null 2>&1; then
        timeout_cmd="gtimeout 300"
    elif command -v timeout >/dev/null 2>&1; then
        timeout_cmd="timeout 300"
    fi
    
    if [ -n "$timeout_cmd" ]; then
    if [ -n "$timeout_cmd" ]; then
        if $timeout_cmd $test_cmd > "test_output_${branch//\//_}.log" 2>&1; then
            local end_time=$(date +%s)
            local duration=$((end_time - start_time))
            log_success "Branch '$branch' tests passed (${duration}s)"
            
            # Copy report if it exists
            if [ "$generate_report" = "true" ] && [ -f "tests/cli/git_test_report.json" ]; then
                cp "tests/cli/git_test_report.json" "$report_file"
            fi
            
            return 0
        else
            local end_time=$(date +%s)
            local duration=$((end_time - start_time))
            log_error "Branch '$branch' tests failed (${duration}s)"
            
            # Show last few lines of error
            echo "Last 10 lines of output:"
            tail -10 "test_output_${branch//\//_}.log" | sed 's/^/  /'
            
            return 1
        fi
    else
        # Run without timeout on systems that don't have it
        if $test_cmd > "test_output_${branch//\//_}.log" 2>&1; then
        local end_time=$(date +%s)
        local duration=$((end_time - start_time))
        log_success "Branch '$branch' tests passed (${duration}s)"
        
        # Copy report if it exists
        if [ "$generate_report" = "true" ] && [ -f "tests/cli/git_test_report.json" ]; then
            cp "tests/cli/git_test_report.json" "$report_file"
        fi
        
        return 0
    else
        local end_time=$(date +%s)
        local duration=$((end_time - start_time))
        log_error "Branch '$branch' tests failed (${duration}s)"
        
        # Show last few lines of error
        echo "Last 10 lines of output:"
        tail -10 "test_output_${branch//\//_}.log" | sed 's/^/  /'
        
        return 1
    fi
}

generate_comparison_report() {
    local branches=("$@")
    
    log_info "Generating cross-branch comparison report..."
    
    cat > "multi_branch_test_comparison.md" << EOF
# TreeWidzard Multi-Branch Test Comparison Report

**Generated:** $(date)
**Tested Branches:** ${branches[*]}

## Summary

| Branch | Status | Duration | Test Files | Notes |
|--------|--------|----------|------------|-------|
EOF

    for branch in "${branches[@]}"; do
        local status="❌ Failed"
        local log_file="test_output_${branch//\//_}.log"
        local duration="N/A"
        local test_files="Missing"
        local notes=""
        
        if [ -f "$log_file" ]; then
            if grep -q "All tests passed" "$log_file" 2>/dev/null; then
                status="✅ Passed"
            elif grep -q "Some tests failed" "$log_file" 2>/dev/null; then
                status="⚠️ Partial"
            fi
            
            # Extract duration if available
            duration=$(grep -o "Duration: [0-9.]*" "$log_file" | head -1 | cut -d' ' -f2 || echo "N/A")
            
            # Check test availability
            git checkout "$branch" 2>/dev/null
            if [ -f "tests/cli/run_tests.sh" ]; then
                test_files="Available"
            else
                notes="Test infrastructure missing"
            fi
        else
            notes="No test output"
        fi
        
        echo "| $branch | $status | ${duration}s | $test_files | $notes |" >> "multi_branch_test_comparison.md"
    done
    
    cat >> "multi_branch_test_comparison.md" << EOF

## Detailed Results

EOF

    for branch in "${branches[@]}"; do
        local log_file="test_output_${branch//\//_}.log"
        local report_file="tests/cli/git_test_report_${branch//\//_}.json"
        
        echo "### Branch: $branch" >> "multi_branch_test_comparison.md"
        echo "" >> "multi_branch_test_comparison.md"
        
        if [ -f "$log_file" ]; then
            echo "**Test Output Summary:**" >> "multi_branch_test_comparison.md"
            echo '```' >> "multi_branch_test_comparison.md"
            tail -20 "$log_file" >> "multi_branch_test_comparison.md"
            echo '```' >> "multi_branch_test_comparison.md"
            echo "" >> "multi_branch_test_comparison.md"
        fi
        
        if [ -f "$report_file" ]; then
            echo "**Test Statistics:**" >> "multi_branch_test_comparison.md"
            echo '```json' >> "multi_branch_test_comparison.md"
            jq '.summary' "$report_file" 2>/dev/null >> "multi_branch_test_comparison.md" || echo "Report parsing failed"
            echo '```' >> "multi_branch_test_comparison.md"
            echo "" >> "multi_branch_test_comparison.md"
        fi
    done
    
    log_success "Comparison report generated: multi_branch_test_comparison.md"
}

show_summary_table() {
    echo
    echo -e "${CYAN}=== MULTI-BRANCH TEST SUMMARY ===${NC}"
    echo
    printf "%-25s %-10s %-15s %s\n" "Branch" "Status" "Duration" "Notes"
    printf "%-25s %-10s %-15s %s\n" "$(printf '%*s' 25 | tr ' ' '-')" "$(printf '%*s' 10 | tr ' ' '-')" "$(printf '%*s' 15 | tr ' ' '-')" "$(printf '%*s' 20 | tr ' ' '-')"
    
    for result in "${BRANCH_RESULTS[@]}"; do
        echo "$result"
    done
    
    echo
    log_info "Total branches tested: $TOTAL_BRANCHES"
    log_success "Branches passed: $PASSED_BRANCHES"
    
    if [ ${#FAILED_BRANCHES[@]} -gt 0 ]; then
        log_error "Branches failed: ${FAILED_BRANCHES[*]}"
    fi
    
    local success_rate=$((PASSED_BRANCHES * 100 / TOTAL_BRANCHES))
    echo -e "${CYAN}Overall Success Rate: ${success_rate}%${NC}"
}

main() {
    local branches_to_test=""
    local exclude_branches=""
    local quick_mode="false"
    local generate_report="false"
    local compare_mode="false"
    local show_summary="false"
    local test_type="all"
    local no_checkout="false"
    
    # Parse command line arguments
    while [[ $# -gt 0 ]]; do
        case $1 in
            -h|--help)
                show_help
                exit 0
                ;;
            -b|--branches)
                branches_to_test="$2"
                shift 2
                ;;
            -e|--exclude)
                exclude_branches="$2"
                shift 2
                ;;
            -q|--quick)
                quick_mode="true"
                shift
                ;;
            -r|--report)
                generate_report="true"
                shift
                ;;
            -c|--compare)
                compare_mode="true"
                shift
                ;;
            -s|--summary)
                show_summary="true"
                shift
                ;;
            --shell-only)
                test_type="shell"
                shift
                ;;
            --python-only)
                test_type="python"
                shift
                ;;
            --no-checkout)
                no_checkout="true"
                shift
                ;;
            *)
                log_error "Unknown option: $1"
                show_help
                exit 1
                ;;
        esac
    done
    
    log_info "Starting TreeWidzard Multi-Branch Git CLI Tests"
    log_info "Repository: $(pwd)"
    
    # Backup current state
    local original_branch=$(get_current_branch)
    local stash_state="clean"
    
    if [ "$no_checkout" = "false" ]; then
        stash_state=$(backup_current_state)
        log_info "Original branch: $original_branch"
    fi
    
    # Determine branches to test
    local branches=()
    if [ -n "$branches_to_test" ]; then
        IFS=',' read -ra branches <<< "$branches_to_test"
    else
        while IFS= read -r line; do 
            branches+=("$line")
        done <<< "$(get_all_branches)"
    fi
    
    # Remove excluded branches
    if [ -n "$exclude_branches" ]; then
        IFS=',' read -ra excluded <<< "$exclude_branches"
        for exclude in "${excluded[@]}"; do
            branches=(${branches[@]/$exclude})
        done
    fi
    
    # Handle no-checkout mode
    if [ "$no_checkout" = "true" ]; then
        branches=("$original_branch")
        log_info "Testing current branch only: $original_branch"
    fi
    
    log_info "Branches to test: ${branches[*]}"
    
    # Test each branch
    for branch in "${branches[@]}"; do
        if [ -z "$branch" ]; then continue; fi
        
        TOTAL_BRANCHES=$((TOTAL_BRANCHES + 1))
        
        echo
        log_branch "======= Testing Branch: $branch ======="
        
        # Switch to branch (unless no-checkout mode)
        if [ "$no_checkout" = "false" ]; then
            if ! git checkout "$branch" 2>/dev/null; then
                log_error "Failed to checkout branch: $branch"
                FAILED_BRANCHES+=("$branch")
                BRANCH_RESULTS+=("$(printf "%-25s %-10s %-15s %s" "$branch" "❌ FAILED" "N/A" "Checkout failed")")
                continue
            fi
        fi
        
        # Run tests for this branch
        local start_time=$(date +%s)
        if run_branch_tests "$branch" "$test_type" "$quick_mode" "$generate_report"; then
            local end_time=$(date +%s)
            local duration=$((end_time - start_time))
            PASSED_BRANCHES=$((PASSED_BRANCHES + 1))
            BRANCH_RESULTS+=("$(printf "%-25s %-10s %-15s %s" "$branch" "✅ PASSED" "${duration}s" "All tests passed")")
        else
            local end_time=$(date +%s)
            local duration=$((end_time - start_time))
            FAILED_BRANCHES+=("$branch")
            BRANCH_RESULTS+=("$(printf "%-25s %-10s %-15s %s" "$branch" "❌ FAILED" "${duration}s" "Tests failed")")
        fi
    done
    
    # Restore original state
    if [ "$no_checkout" = "false" ]; then
        restore_state "$original_branch" "$stash_state"
    fi
    
    # Generate comparison report
    if [ "$compare_mode" = "true" ]; then
        generate_comparison_report "${branches[@]}"
    fi
    
    # Show summary
    if [ "$show_summary" = "true" ] || [ $TOTAL_BRANCHES -gt 1 ]; then
        show_summary_table
    fi
    
    # Cleanup log files
    log_info "Cleaning up temporary files..."
    rm -f test_output_*.log
    
    # Exit with appropriate code
    if [ ${#FAILED_BRANCHES[@]} -eq 0 ]; then
        log_success "All branch tests completed successfully!"
        exit 0
    else
        log_error "Some branch tests failed: ${FAILED_BRANCHES[*]}"
        exit 1
    fi
}

# Verify we're in the correct directory
if [ ! -f "CMakeLists.txt" ]; then
    log_error "Please run this script from the TreeWidzard-development root directory"
    exit 1
fi

main "$@"