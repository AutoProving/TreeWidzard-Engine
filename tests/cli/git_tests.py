#!/usr/bin/env python3
"""
TreeWidzard Git CLI Test Suite - Python Version
Advanced automated testing for git operations with detailed reporting
"""

import subprocess
import sys
import os
import re
import json
import time
from datetime import datetime
from pathlib import Path
from typing import Dict, List, Tuple, Optional

class Colors:
    RED = '\033[0;31m'
    GREEN = '\033[0;32m'
    YELLOW = '\033[1;33m'
    BLUE = '\033[0;34m'
    PURPLE = '\033[0;35m'
    CYAN = '\033[0;36m'
    WHITE = '\033[1;37m'
    NC = '\033[0m'  # No Color

class GitTestRunner:
    def __init__(self):
        self.tests_run = 0
        self.tests_passed = 0
        self.tests_failed = 0
        self.test_results = []
        self.start_time = time.time()
        
    def log(self, level: str, message: str, color: str = Colors.NC):
        timestamp = datetime.now().strftime("%H:%M:%S")
        print(f"{color}[{timestamp}] [{level}]{Colors.NC} {message}")
        
    def log_info(self, message: str):
        self.log("INFO", message, Colors.BLUE)
        
    def log_success(self, message: str):
        self.log("PASS", message, Colors.GREEN)
        self.tests_passed += 1
        
    def log_error(self, message: str):
        self.log("FAIL", message, Colors.RED)
        self.tests_failed += 1
        
    def log_warning(self, message: str):
        self.log("WARN", message, Colors.YELLOW)

    def run_command(self, command: str, capture_output: bool = True) -> Tuple[int, str, str]:
        """Run a shell command and return exit code, stdout, stderr"""
        try:
            result = subprocess.run(
                command,
                shell=True,
                capture_output=capture_output,
                text=True,
                timeout=30
            )
            return result.returncode, result.stdout.strip(), result.stderr.strip()
        except subprocess.TimeoutExpired:
            return -1, "", "Command timed out"
        except Exception as e:
            return -1, "", str(e)

    def run_test(self, test_name: str, command: str, expected_exit_code: int = 0) -> bool:
        """Run a test and check exit code"""
        self.tests_run += 1
        self.log_info(f"Running test: {test_name}")
        
        exit_code, stdout, stderr = self.run_command(command)
        
        success = exit_code == expected_exit_code
        result = {
            "name": test_name,
            "command": command,
            "expected_exit_code": expected_exit_code,
            "actual_exit_code": exit_code,
            "stdout": stdout,
            "stderr": stderr,
            "success": success,
            "timestamp": datetime.now().isoformat()
        }
        self.test_results.append(result)
        
        if success:
            self.log_success(test_name)
        else:
            self.log_error(f"{test_name} (expected {expected_exit_code}, got {exit_code})")
            if stderr:
                print(f"  Error: {stderr}")
                
        return success

    def run_test_with_pattern(self, test_name: str, command: str, expected_pattern: str) -> bool:
        """Run a test and check if output matches pattern"""
        self.tests_run += 1
        self.log_info(f"Running test: {test_name}")
        
        exit_code, stdout, stderr = self.run_command(command)
        
        success = re.search(expected_pattern, stdout) is not None
        result = {
            "name": test_name,
            "command": command,
            "expected_pattern": expected_pattern,
            "stdout": stdout,
            "stderr": stderr,
            "success": success,
            "timestamp": datetime.now().isoformat()
        }
        self.test_results.append(result)
        
        if success:
            self.log_success(test_name)
        else:
            self.log_error(f"{test_name} (pattern '{expected_pattern}' not found)")
            print(f"  Output: {stdout}")
            
        return success

    def get_git_info(self) -> Dict:
        """Collect comprehensive git repository information"""
        info = {}
        
        # Basic info
        _, info['current_branch'] = self.run_command("git branch --show-current")
        _, info['remote_url'] = self.run_command("git remote get-url origin")
        _, info['commit_count'] = self.run_command("git rev-list --count HEAD")
        _, info['last_commit'] = self.run_command("git log -1 --pretty=format:'%H %s'")
        
        # Branch info
        _, branches_output = self.run_command("git branch -a")
        info['branches'] = [b.strip().replace('* ', '') for b in branches_output.split('\n') if b.strip()]
        
        # Status info
        _, status_output = self.run_command("git status --porcelain")
        info['untracked_files'] = len([l for l in status_output.split('\n') if l.startswith('??')])
        info['modified_files'] = len([l for l in status_output.split('\n') if l.startswith(' M')])
        info['staged_files'] = len([l for l in status_output.split('\n') if l.startswith('M ')])
        
        # Unpushed commits
        _, unpushed_output = self.run_command("git log --oneline main..HEAD")
        info['unpushed_commits'] = len([l for l in unpushed_output.split('\n') if l.strip()])
        
        return info

    def run_comprehensive_tests(self):
        """Run all git tests"""
        self.log_info("Starting TreeWidzard Git CLI Test Suite (Python)")
        self.log_info(f"Repository: {os.getcwd()}")
        
        # Collect git info
        git_info = self.get_git_info()
        self.log_info(f"Current branch: {git_info['current_branch']}")
        self.log_info(f"Total commits: {git_info['commit_count']}")
        
        # Core Git Tests
        self.log_info("=== Core Git Repository Tests ===")
        self.run_test("Git repository detection", "git rev-parse --git-dir")
        self.run_test("Git configuration valid", "git config --list")
        self.run_test("Remote origin accessible", "git ls-remote --heads origin")
        
        # Branch Tests
        self.log_info("=== Branch Structure Tests ===")
        self.run_test_with_pattern("Current branch is GPU feature", "git branch --show-current", "feature/gpu-acceleration")
        self.run_test("Main branch exists", "git show-ref --verify --quiet refs/heads/main")
        self.run_test("Optimized branch exists", "git show-ref --verify --quiet refs/heads/optimized")
        self.run_test("Feature branch exists", "git show-ref --verify --quiet refs/heads/feature/gpu-acceleration")
        
        # Working Tree Tests
        self.log_info("=== Working Tree Tests ===")
        self.run_test_with_pattern("Working tree clean", "git status --porcelain", "^$")
        self.run_test("No merge conflicts", "git diff --check")
        
        # Commit History Tests
        self.log_info("=== Commit History Tests ===")
        self.run_test("Git log accessible", "git log --oneline -1")
        self.run_test("Commits have valid authors", "git log --pretty=format:'%an' -10")
        self.run_test("No empty commit messages", "git log --pretty=format:'%s' -10 | grep -v '^$'")
        
        # Branch Relationship Tests
        self.log_info("=== Branch Relationship Tests ===")
        self.run_test("Optimized diverged from main", "git merge-base --is-ancestor main optimized")
        self.run_test("GPU branch diverged from optimized", "git merge-base --is-ancestor optimized feature/gpu-acceleration")
        
        # File and Repository Tests
        self.log_info("=== Repository Health Tests ===")
        self.run_test("Important files tracked", "git ls-files | grep -E '\\.(cpp|h|cmake)$'")
        self.run_test("No large binary files", "git rev-list --objects --all | git cat-file --batch-check='%(objecttype) %(objectname) %(objectsize) %(rest)' | awk '/^blob/ {if($3 > 1048576) {print $4 \" (\" $3 \" bytes)\"; exit 1}}'", 1)
        
        # Security Tests
        self.log_info("=== Security Tests ===")
        self.run_test("No passwords in commits", "git log --all --full-history -- | grep -i password", 1)
        self.run_test("No API keys in commits", "git log --all --full-history -- | grep -iE '(api_key|secret|token)', 1)
        
        # Performance Tests
        self.log_info("=== Performance Tests ===")
        _, repo_size = self.run_command("du -sh .git")
        self.log_info(f"Repository size: {repo_size}")
        
        # Custom TreeWidzard Tests
        self.log_info("=== TreeWidzard Specific Tests ===")
        self.run_test("CMakeLists.txt exists", "test -f CMakeLists.txt")
        self.run_test("GPU directory exists", "test -d GPU")
        self.run_test("Main.cpp exists", "test -f main.cpp")
        
        # Unpushed commits warning
        if git_info['unpushed_commits'] > 0:
            self.log_warning(f"Found {git_info['unpushed_commits']} unpushed commits on current branch")
        
        # Summary
        self.log_info("=== Test Summary ===")
        duration = time.time() - self.start_time
        self.log_info(f"Tests run: {self.tests_run}")
        self.log_info(f"Duration: {duration:.2f} seconds")
        
        if self.tests_passed > 0:
            self.log_success(f"Tests passed: {self.tests_passed}")
        if self.tests_failed > 0:
            self.log_error(f"Tests failed: {self.tests_failed}")
            
        # Generate report
        self.generate_report()
        
        return self.tests_failed == 0

    def generate_report(self):
        """Generate a detailed test report"""
        report_file = "tests/cli/git_test_report.json"
        report_data = {
            "timestamp": datetime.now().isoformat(),
            "summary": {
                "tests_run": self.tests_run,
                "tests_passed": self.tests_passed,
                "tests_failed": self.tests_failed,
                "success_rate": (self.tests_passed / self.tests_run * 100) if self.tests_run > 0 else 0
            },
            "git_info": self.get_git_info(),
            "test_results": self.test_results
        }
        
        os.makedirs(os.path.dirname(report_file), exist_ok=True)
        with open(report_file, 'w') as f:
            json.dump(report_data, f, indent=2)
            
        self.log_info(f"Detailed report saved to: {report_file}")

def main():
    # Check if we're in the right directory
    if not os.path.exists("CMakeLists.txt") or not os.path.exists("GPU"):
        print(f"{Colors.RED}Error: Please run this script from the TreeWidzard-development root directory{Colors.NC}")
        sys.exit(1)
    
    runner = GitTestRunner()
    success = runner.run_comprehensive_tests()
    
    sys.exit(0 if success else 1)

if __name__ == "__main__":
    main()