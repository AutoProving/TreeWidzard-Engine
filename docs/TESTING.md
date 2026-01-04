# Testing

TreeWidzard has three layers of tests:

- **C++ unit/integration tests (GoogleTest)** via `ctest`
- **Certificate end-to-end tests** (generate `.twzcert` + replay with `-checkcert`)
- **CLI smoke / repo-health tests** used by CI and the Docker build

## Run Locally (C++)

From the repo root:

```bash
cmake -S . -B build
cmake --build build -j 8
ctest --test-dir build --output-on-failure -j 8
```

Run only certificate tests:

```bash
ctest --test-dir build --output-on-failure -R '^Certificates\\.' -j 8
```

List all registered tests (and the total count):

```bash
ctest --test-dir build -N
```

Current counts (from `ctest -N` on this repo build):
- Total: **44** tests
- Certificate suite (`-R '^Certificates\\.'`): **12** tests

## Test Inventory (What Is Covered)

### Unit Tests (`unit_tests`)

**`tests/unit/test_bag_operations.cpp`**
- `BagTest.*`: bag creation, size, ordering/comparison, hashing, duplicate handling, edge operations.

**`tests/unit/test_witness_cache.cpp`**
- `WitnessCacheTest.*`: cache initialization, capacity behavior, clearing, multi-cache behavior (stress-style; can take ~60s per test depending on platform).

### Integration Tests (`integration_tests`)

**`tests/integration/test_atp_workflow.cpp`**
- `ATPIntegrationTest.*`: conjecture file creation/parsing, width/flag API behavior, search strategy plugin availability, width comparisons.

**`tests/integration/test_certificates.cpp`**
- **Positive (acceptance)**
  - `Certificates.BFS_SAT_GeneratesAndChecks`: BFS emits a SAT certificate and `-checkcert` accepts.
  - `Certificates.ISO_SAT_GeneratesAndChecks`: ISO-BFS emits a canonized SAT certificate and `-checkcert` accepts.
  - `Certificates.PBFS_SAT_GeneratesAndChecks`: Parallel BFS emits a SAT certificate and `-checkcert` accepts.
  - `Certificates.PISO_SAT_GeneratesAndChecks`: Parallel ISO-BFS emits a SAT certificate and `-checkcert` accepts.
  - `Certificates.Premise_BFS_SAT_GeneratesAndChecks`: `-premise` SAT certificate is accepted and has `H PREMISE 1`.
  - `Certificates.BFS_NO_GeneratesAndChecks`: BFS emits a NOT_SATISFIED certificate and `-checkcert` accepts it (bad-state replay).
- **Negative / robustness (rejection)**
  - `Certificates.Negative_TruncatedSATRejected`: truncated SAT certificate is rejected (closure violation).
  - `Certificates.Negative_WrongPropertyRejected`: certificate checked against a different conjecture is rejected (`H PROP_HASH` mismatch).
  - `Certificates.Negative_TamperedHeaderRejected`: tampering `H DPCORES_HASH` is rejected (plugin fingerprint mismatch).
  - `Certificates.Negative_PremiseRequiresImplication`: `H PREMISE 1` is rejected if the conjecture is not an implication.
  - `Certificates.Negative_InvalidOpRejected`: invalid transition encoding (e.g., out-of-range label) is rejected.
  - `Certificates.Negative_InvalidJoinMapRejected`: invalid `JOIN MAP` (not a permutation) is rejected.

### Legacy/Conjecture Tests (`run_tests`)

**`tests/test_conjecture.cpp`**
- `ConjectureTest.*`: expression parsing, nesting, boolean/comparison/arithmetic evaluation, invalid operator handling, structural validation.
- `PropertyAssignmentTest.*`: variable/property assignment parsing and parameter handling.
- `ConjectureIntegrationTest.*`: kernel-level integration and variable→property mapping.

## CLI Smoke Tests (Docker/CI)

**`tests/cli/smoke_test.sh`** (runs in `Dockerfile`)
- Verifies `treewidzard --help` runs and prints expected help text (and does not crash).

**`tests/cli/*`**
- Repo-health / Git workflow checks (separate from solver/certificate correctness).
- Entry point: `./tests/cli/run_tests.sh`.
- Some checks may require git remotes / network access depending on configuration.

## Optional: Benchmarks / GPU Targets

**`tests/benchmarks/*`**
- Built only if the `benchmark` dependency is found.

**`tests/GPUTests.cpp`**
- Built/used only under GPU-specific CMake configuration (see `cmake/GPUSupport.cmake`).
