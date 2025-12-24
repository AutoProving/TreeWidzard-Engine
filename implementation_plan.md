# Implementation Plan: CI Pipeline Improvements

## Goal
Now that we have a robust Docker image, we should use it in our Continuous Integration (CI) pipeline. This validates that the Docker image works correctly and standardizes our test environment.

## Proposed Changes

### 1. [MODIFY] `.github/workflows/docker-publish.yml`
- **Add Tests**: Update the workflow to run the test suite *inside* the container after building it.
- **Command**: `docker run --rm treewidzard:local-test ./tests/cli/run_tests.sh` (or equivalent).

### 2. [MODIFY] `tests/cli/run_tests.sh` (Optional)
- Ensure the test script is robust enough to run inside the container (e.g., doesn't depend on absolute paths on the host).

## Verification Plan
1.  **Local Run**: `docker run --rm treewidzard:clean-build-test ./tests/cli/run_tests.sh`
    - Verify that the tests pass inside the container.
2.  **Commit**: Push changes to trigger the GitHub Action (if possible, or just verified locally).
