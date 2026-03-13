#!/bin/bash

# Verify GitHub Actions workflows are fixed across all branches

echo "🔍 Verifying GitHub Actions Workflow Fixes Across All Branches"
echo "=============================================================="

check_branch() {
    local branch=$1
    echo
    echo "📍 Checking branch: $branch"
    
    if git show "$branch:.github/workflows/git-cli-tests.yml" >/dev/null 2>&1; then
        echo "✅ Workflow file exists on $branch"
        
        # Check for updated action versions
        local setup_python=$(git show "$branch:.github/workflows/git-cli-tests.yml" | grep "setup-python@" | head -1)
        local upload_artifact=$(git show "$branch:.github/workflows/git-cli-tests.yml" | grep "upload-artifact@" | head -1)
        local github_script=$(git show "$branch:.github/workflows/git-cli-tests.yml" | grep "github-script@" | head -1)
        
        # Check for proper shell test execution
        local shell_test_dir=$(git show "$branch:.github/workflows/git-cli-tests.yml" | grep -A5 "Run Shell Git Tests" | grep "cd tests/cli" || echo "")
        
        echo "   Actions versions:"
        if [[ $setup_python == *"@v5"* ]]; then
            echo "   ✅ setup-python@v5"
        elif [[ $setup_python == *"@v4"* ]]; then
            echo "   ⚠️  setup-python@v4 (should be v5)"
        else
            echo "   ❓ setup-python version unclear"
        fi
        
        if [[ $upload_artifact == *"@v4"* ]]; then
            echo "   ✅ upload-artifact@v4"
        elif [[ $upload_artifact == *"@v3"* ]]; then
            echo "   ⚠️  upload-artifact@v3 (should be v4)"
        else
            echo "   ❓ upload-artifact version unclear"
        fi
        
        if [[ $github_script == *"@v7"* ]]; then
            echo "   ✅ github-script@v7"
        elif [[ $github_script == *"@v6"* ]]; then
            echo "   ⚠️  github-script@v6 (should be v7)"
        else
            echo "   ❓ github-script version unclear"
        fi
        
        # Check shell test execution path
        if [[ -z "$shell_test_dir" ]]; then
            echo "   ✅ Shell tests run from project root"
        else
            echo "   ⚠️  Shell tests still use 'cd tests/cli' (should run from root)"
        fi
        
        # Check for fail-fast setting
        local fail_fast=$(git show "$branch:.github/workflows/git-cli-tests.yml" | grep "fail-fast: false" || echo "")
        if [[ -n "$fail_fast" ]]; then
            echo "   ✅ fail-fast: false configured"
        else
            echo "   ⚠️  fail-fast not configured"
        fi
        
    else
        echo "❌ No workflow file found on $branch"
    fi
}

# Get current branch
original_branch=$(git branch --show-current)

# Check all branches
branches=("main" "optimized")

for branch in "${branches[@]}"; do
    check_branch "$branch"
done

echo
echo "📊 Summary"
echo "=========="
echo "Checked branches: ${branches[*]}"
echo "Original branch: $original_branch"
echo
echo "🎯 Expected Status After Fixes:"
echo "✅ All branches should have:"
echo "   - setup-python@v5"
echo "   - upload-artifact@v4" 
echo "   - github-script@v7"
echo "   - fail-fast: false"
echo "   - Shell tests run from project root"
echo
echo "💡 If any branch shows warnings, the fixes need to be applied to that branch."
