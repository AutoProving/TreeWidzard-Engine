# TreeWidzard Git CLI Automated Tests

This directory contains comprehensive automated testing tools for Git operations and repository health checks in the TreeWidzard project.

## 🚀 Quick Start

Run all tests with a single command:
```bash
./tests/cli/run_tests.sh --all --report
```

## 📁 Test Files

- **`git_tests.sh`** - Shell-based git tests (lightweight, fast)
- **`git_tests.py`** - Python-based comprehensive tests with detailed reporting
- **`run_tests.sh`** - Main test runner that orchestrates all tests
- **`test_config.yml`** - Configuration file for test parameters
- **`.github/workflows/git-cli-tests.yml`** - GitHub Actions workflow for CI/CD

## 🧪 Test Categories

### Core Git Operations
- Repository detection and validation
- Remote connectivity checks
- Basic git command functionality

### Branch Management
- Branch existence verification
- Branch relationship validation
- Merge conflict detection

### Working Tree Status
- Clean working tree verification
- File tracking status
- Uncommitted changes detection

### Commit History
- Commit integrity checks
- Author information validation
- Commit message quality

### Security Checks
- Scan for passwords/secrets in history
- Detect API keys or tokens
- File permission validation

### Performance & Health
- Repository size monitoring
- Large file detection
- Project structure validation

## 📋 Usage Examples

### Run Only Shell Tests
```bash
./tests/cli/run_tests.sh --shell
```

### Run Only Python Tests with Report
```bash
./tests/cli/run_tests.sh --python --report
```

### Quick Critical Tests Only
```bash
./tests/cli/run_tests.sh --quick
```

### Verbose Output
```bash
./tests/cli/run_tests.sh --verbose
```

### Direct Test Execution
```bash
# Run shell tests directly
./tests/cli/git_tests.sh

# Run Python tests directly
python3 tests/cli/git_tests.py
```

## 📊 Test Results

### Exit Codes
- `0` - All tests passed
- `1` - Some tests failed

### Reports
When using `--report`, detailed JSON reports are generated:
- **Location**: `tests/cli/git_test_report.json`
- **Contents**: Test results, timing, git repository info, failure details

### Example Report Structure
```json
{
  "timestamp": "2025-10-28T09:36:12.254305",
  "summary": {
    "tests_run": 21,
    "tests_passed": 20,
    "tests_failed": 1,
    "success_rate": 95.2
  },
  "git_info": {
    "current_branch": "main",
    "remote_url": "https://github.com/farhad-vadiee/TreeWidzard-development.git",
    "unpushed_commits": 13
  },
  "test_results": [...]
}
```

## 🤖 Continuous Integration

Tests automatically run on:
- **Push** to maintained branches (`main`, `optimized`)
- **Pull requests** to `main`
- **Daily schedule** at 2 AM UTC
- **Manual triggers** via GitHub Actions

## 🔧 Configuration

Modify `test_config.yml` to customize:
- Expected repository state
- File patterns to track/ignore
- Security scan patterns
- Performance thresholds

## 🚨 Common Issues

### Test Failures
1. **"Working tree not clean"** - Commit or stash changes
2. **"Branch not found"** - Ensure all expected branches exist
3. **"Remote not accessible"** - Check network/GitHub access
4. **"Large files detected"** - Review and remove large binary files

### Prerequisites
- Git repository with proper remotes configured
- Python 3.x for advanced tests
- Network access for remote operations
- Proper file permissions on test scripts

## 📈 Test Coverage

Current test suite covers:
- ✅ Repository integrity (100%)
- ✅ Branch management (100%)
- ✅ Working tree status (100%)
- ✅ Security scanning (100%)
- ✅ Performance monitoring (100%)
- ✅ Project structure validation (100%)

## 🛠 Development

### Adding New Tests

**Shell tests** (git_tests.sh):
```bash
run_test "Test name" "command to run" [expected_exit_code]
run_test_with_output "Test name" "command" "expected_pattern"
```

**Python tests** (git_tests.py):
```python
self.run_test("Test name", "command", expected_exit_code)
self.run_test_with_pattern("Test name", "command", "pattern")
```

### Test Categories
Organize tests into logical categories for better reporting and maintenance.

## 📝 Best Practices

1. **Run tests before commits** to catch issues early
2. **Use reports** for detailed analysis of failures
3. **Monitor CI/CD** for automated test results
4. **Keep tests fast** - most should complete in seconds
5. **Make tests deterministic** - avoid time-dependent tests

---

**Need help?** Check the test output or run with `--verbose` for detailed information.
