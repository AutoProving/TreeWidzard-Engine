# TreeWidzard Quick Start Guide

## Building

```bash
mkdir -p build
cd build
cmake ..
cmake --build . -j $(sysctl -n hw.ncpu)
cd ..
```

## Running TreeWidzard

### Easy Way (Recommended)
Use the wrapper script that automatically sets up the environment:

```bash
./treewidzard.sh -atp tw=3 BreadthFirstSearch examples/conjectures/simple_implies.txt
```

### Manual Way
Set environment variables and run directly:

```bash
export TREEWIDZARD_SEARCHSTRATEGIES="$(pwd)/build/SearchStrategies"
./build/treewidzard -atp tw=3 BreadthFirstSearch examples/conjectures/simple_implies.txt
```

## Certificates (ATP Mode)

TreeWidzard can emit a replayable certificate for `PROPERTY SATISFIED` / `PROPERTY NOT SATISFIED`
that can be validated by a separate checker (`-checkcert`).

### Generate a certificate

```bash
./treewidzard.sh -atp tw=2 BreadthFirstSearch examples/conjectures/simple_implies.txt \
  -cert proof.twzcert
```

### Check a certificate

```bash
./treewidzard.sh -checkcert examples/conjectures/simple_implies.txt proof.twzcert
```

Notes:
- Certificates record a property hash (`H PROP_HASH ...`) and a DP-core plugin fingerprint (`H DPCORES_HASH ...`); the checker rejects mismatches.
- If you use `-premise`, the checker enforces premise-pruned closure and requires the conjecture to be an implication.

## Available Search Strategies

- `BreadthFirstSearch` - Standard breadth-first search
- `ParallelBreadthFirstSearch` - Multi-threaded BFS
- `IsomorphismBreadthFirstSearch` - BFS with isomorphism checking
- `ParallelIsomorphismBreadthFirstSearch` - Parallel BFS with isomorphism

## Example Conjectures

Located in `examples/conjectures/`:
- `simple_implies.txt` - MaxDegree(4) implies ChromaticNumber(4)
- `independence_bound.txt` - Independence number bounds
- More examples available in the directory

## Command Line Options

```
ATP Mode (Search for counterexamples):
  ./treewidzard.sh -atp [Width] [Flags] <search_strategy> <conjecture_file>

Width Parameters:
  tw = N    Tree-width at most N
  pw = N    Path-width at most N

Flags:
  -pl       Print loop (show progress)
  -ps       Print states
  -st       Print state tree decomposition
  -nthreads N   Number of threads for parallel search

Model Checking:
  ./treewidzard.sh -modelcheck PACE [Flags] <property> <graph> <decomposition>
```

## Troubleshooting

**Error: "Search method X was not found"**
- Make sure you built the search strategies: `cmake --build build --target BreadthFirstSearch`
- Use the wrapper script `./treewidzard.sh` which sets paths automatically

**Slow performance**
- Try parallel search strategies: `ParallelBreadthFirstSearch`
- Increase thread count: `-nthreads 8`
- Reduce tree-width if possible

## Next Steps

See `optimization_plan.md` in `.gemini/antigravity/brain/` for:
- Performance optimization guide
- Testing framework setup
- Advanced usage examples

## Testing

See `docs/TESTING.md`. Quick run: `ctest --test-dir build --output-on-failure`.
