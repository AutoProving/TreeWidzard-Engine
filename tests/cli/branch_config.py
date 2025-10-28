#!/usr/bin/env python3
"""
Branch-aware test configuration for TreeWidzard Git CLI tests
Automatically adjusts test expectations based on current branch
"""

import subprocess
import sys

def get_current_branch():
    """Get the current git branch"""
    try:
        result = subprocess.run(['git', 'branch', '--show-current'], 
                              capture_output=True, text=True)
        return result.stdout.strip()
    except:
        return "unknown"

def get_branch_config():
    """Get branch-specific test configuration"""
    branch = get_current_branch()
    
    # Base configuration for all branches
    config = {
        "run_security_tests": True,
        "run_performance_tests": True,
        "run_structure_tests": True,
        "run_git_health_tests": True,
        "expected_current_branch": branch,
        "require_gpu_directory": False,
        "require_feature_files": False,
        "check_branch_relationships": True
    }
    
    # Branch-specific configurations
    if branch == "main":
        config.update({
            "expected_current_branch": "main",
            "require_gpu_directory": False,
            "check_gpu_branch_relationship": False,
            "test_description": "Core repository tests and basic structure validation"
        })
        
    elif branch == "optimized":
        config.update({
            "expected_current_branch": "optimized",
            "require_gpu_directory": True,  # GPU features were merged
            "check_gpu_branch_relationship": True,
            "run_performance_tests": True,
            "test_description": "Performance optimization validation and enhanced features"
        })
        
    elif branch.startswith("feature/"):
        config.update({
            "expected_current_branch": branch,
            "require_gpu_directory": True if "gpu" in branch else False,
            "require_feature_files": True,
            "check_all_relationships": True,
            "test_description": f"Feature-specific tests for {branch}"
        })
        
    return config

if __name__ == "__main__":
    import json
    config = get_branch_config()
    print(json.dumps(config, indent=2))