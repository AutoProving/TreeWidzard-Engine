# TreeWidzard Repository - Branch Structure

## Branch Overview

This repository currently maintains two primary long-lived branches:

1. **`main`** - Production baseline
   - Stable, release-oriented TreeWidzard code
   - CPU-only solver and tooling
   - Documentation and tests expected to stay green

2. **`optimized`** - Performance work
   - CPU performance improvements
   - Memory and throughput optimizations
   - Candidate changes for later merge into `main`

Feature branches may still exist temporarily for isolated work, but they are not part of the maintained release structure.

## Development Flow

1. Start feature work from `main` or `optimized`.
2. Validate performance-oriented changes on `optimized`.
3. Merge release-ready changes into `main`.

## Repository Notes

- Build artifacts and generated files should stay out of version control.
- The current release branch structure assumes CPU-only builds.
- Test and CI automation should work from `main` and `optimized`.
