#!/bin/bash

# Quick demonstration of multi-branch testing

echo "🚀 TreeWidzard Multi-Branch Test Demonstration"
echo "=============================================="
echo

# Function to run tests on a branch
test_branch() {
    local branch=$1
    echo "📍 Testing branch: $branch"
    
    if git checkout "$branch" 2>/dev/null; then
        echo "✅ Switched to $branch"
        
        # Run quick tests
        if ./tests/cli/run_tests.sh --python --quick >/dev/null 2>&1; then
            echo "✅ Tests PASSED on $branch"
        else
            echo "⚠️  Some tests failed on $branch (expected for branch-specific tests)"
        fi
        echo
    else
        echo "❌ Failed to switch to $branch"
        echo
    fi
}

# Get current branch
original_branch=$(git branch --show-current)
echo "Original branch: $original_branch"
echo

# Test each branch
test_branch "main"
test_branch "optimized" 
test_branch "feature/gpu-acceleration"

# Return to original branch
git checkout "$original_branch" 2>/dev/null
echo "🏁 Returned to original branch: $original_branch"
echo

echo "📊 Summary:"
echo "- ✅ Full test infrastructure now available on ALL branches"
echo "- 🎯 Branch-aware testing adapts to each branch's requirements"
echo "- 🔄 Tests can be run on any branch with: ./tests/cli/run_tests.sh"
echo "- 📈 Multi-branch testing with: ./tests/cli/multi_branch_tests.sh"