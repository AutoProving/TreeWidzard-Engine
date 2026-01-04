# TreeWidzard User Guide

## Quick Start

### Installation
```bash
git clone https://github.com/AutoProving/TreeWidzard-Engine
cd TreeWidzard-Engine
cmake -BBuild
cmake --build Build
```

### Basic Usage
```bash
./Build/treewidzard --help
```

## Core Concepts

### Tree Decompositions
TreeWidzard operates on graphs with bounded treewidth/pathwidth. A tree decomposition represents the graph structure in a way that enables efficient dynamic programming.

### Conjectures
Conjectures are logical formulas about graph properties. Example:
```
x := ChromaticNumber(4)
Formula
x
```
This asks: "Is the graph 4-colorable?"

### DP-Cores
Dynamic Programming Cores implement specific graph properties:
- `ChromaticNumber(k)`: k-colorability
- `IndependentSet()`: Maximum independent set
- `MaxDegree(k)`: Maximum degree ≥ k
- `VertexCount()`: Number of vertices

## Common Use Cases

### 1. Graph Coloring Verification

**Problem**: Verify if a graph is 4-colorable

**Input Files**:
- `graph.gr` (PACE format graph)
- `graph.td` (tree decomposition)
- `four_colorable.txt` (conjecture)

**Conjecture File** (`four_colorable.txt`):
```
x := ChromaticNumber(4)
Formula
x
```

**Command**:
```bash
./treewidzard -modelcheck PACE four_colorable.txt graph.gr graph.td
```

**Expected Output**:
- If 4-colorable: Shows witness (valid 4-coloring)
- If not 4-colorable: Shows counterexample

### 2. Complex Property Verification

**Problem**: Verify "If maximum degree ≤ 3, then the graph is 4-colorable"

**Conjecture File** (`degree_coloring.txt`):
```
x := MaxDegree(4)
y := ChromaticNumber(4)
Formula
NOT x IMPLIES y
```

**Command**:
```bash
./treewidzard -atp pw=4 -pl -nthreads 10 ParallelBreadthFirstSearch degree_coloring.txt
```

### 3. Independence Number Computation

**Problem**: Compute the independence number of a graph

**Conjecture File** (`independence.txt`):
```
x := IndependentSet()
Formula
x
```

**Command**:
```bash
./treewidzard -modelcheck ITD independence.txt diamond_itd.txt
```

## Input Formats

### 1. PACE Format

**Graph File** (`.gr`):
```
c Comment line
p tw <num_vertices> <num_edges>
<vertex1> <vertex2>
<vertex1> <vertex3>
...
```

**Tree Decomposition** (`.td`):
```
c Comment line  
s td <num_bags> <tree_width> <num_vertices>
b <bag_id> <vertex1> <vertex2> ...
<parent_bag> <child_bag>
...
```

### 2. Instructive Tree Decomposition (ITD)

```
1 Leaf
2 IntroVertex_1(1)
3 IntroVertex_2(2)
4 IntroEdge_1_2(3)
5 ForgetVertex_1(4)
...
```

### 3. Conjecture Syntax

**Variable Assignment**:
```
variable := Property(parameters)
```

**Formula Section**:
```
Formula
<logical_expression>
```

**Supported Operators**:
- Logical: `AND`, `OR`, `NOT`, `IMPLIES`
- Comparison: `<`, `<=`, `>`, `>=`, `==`
- Arithmetic: `+`, `-`, `*`, `/`

## Command Line Options

### Basic Options
- `--help`: Show usage information
- `-modelcheck <format>`: Model checking mode
- `-atp <width_type>=<value>`: Automated theorem proving mode
- `-cert <file>`: Emit a replayable `.twzcert` certificate (ATP mode)
- `-checkcert <conjecture_file> <certificate_file>`: Verify a `.twzcert` certificate

### Width Parameters
- `tw=<k>`: Tree width bound
- `pw=<k>`: Path width bound

### Search Strategy Options
- `BreadthFirstSearch`: Standard BFS
- `ParallelBreadthFirstSearch`: Multi-threaded BFS
- `IsomorphismBreadthFirstSearch`: Isomorphism-aware search
- `ParallelIsomorphismBreadthFirstSearch`: Parallel + isomorphism

### Performance Options
- `-nthreads <n>`: Number of threads for parallel execution
- `-pl`: Enable parallel processing
- `-premise`: Use premise-based reasoning

## Certificates (ATP)

When running in ATP mode, TreeWidzard can optionally emit a certificate file that a
separate checker can validate without trusting the search controller (BFS/ISO-BFS,
pruning, book-keeping).

### Generate

```bash
./treewidzard.sh -atp tw=2 BreadthFirstSearch examples/conjectures/simple_implies.txt \
  -cert proof.twzcert
```

### Verify

```bash
./treewidzard.sh -checkcert examples/conjectures/simple_implies.txt proof.twzcert
```

What the checker validates (high level):
- The certificate matches the conjecture (`H PROP_HASH ...`) and the loaded DP-core plugins (`H DPCORES_HASH ...`).
- For `PROPERTY NOT SATISFIED`: the referenced bad state is actually bad.
- For `PROPERTY SATISFIED`: the logged state set contains no bad state and is closed under all successor operations (and premise-pruned closure when `-premise` was used).

Trust boundary:
- `-checkcert` is independent of the search controller, but it still relies on the DP-core transition/finality implementations and the canonization routine (when enabled).

## Output Interpretation

### Success Case
```
Formula: x
Result: True
Final value: x=1
```

### Failure Case with Counterexample
```
Formula: x
Result: False
CounterExample files generated:
- graph_CounterExample_Graph.txt
- graph_CounterExample_RunTree.txt
- graph_CounterExample_ConcreteDecomposition.txt
```

### Output Files
- **Graph files**: Counterexample graph in various formats
- **RunTree**: Detailed execution trace
- **ConcreteDecomposition**: Tree decomposition used

## Advanced Usage

### Cluster Computing (SLURM)

**Job Script**:
```bash
#!/bin/bash
#SBATCH --account=<account>
#SBATCH --partition=normal
#SBATCH --time=1:00:00
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=32
#SBATCH --mem-per-cpu=4G

./treewidzard -atp pw=3 -pl -nthreads 30 ParallelBreadthFirstSearch conjecture.txt
```

### Custom DP-Cores

To add a new graph property:

1. Create new core class inheriting from `DynamicCore`
2. Implement required methods:
   - `compute()`: DP computation
   - `is_final_witness_set()`: Check final state
   - `inv()`: Invariant computation

3. Compile as shared library
4. Place in DPCores directory

### Performance Tuning

**Memory Optimization**:
- Use smaller width bounds when possible
- Enable isomorphism reduction for symmetric graphs
- Adjust thread count based on available cores

**Speed Optimization**:
- Use parallel search strategies
- Enable premise-based reasoning for complex formulas
- Consider path width instead of tree width for certain graphs

## Troubleshooting

### Common Issues

**"Core not found" Error**:
- Ensure DP-Core libraries are in the correct directory
- Check library naming conventions
- Verify core compilation

**Memory Issues**:
- Reduce width bounds
- Use fewer threads
- Check available system memory

**Timeout Issues**:
- Increase time limits for cluster jobs
- Use more efficient search strategies
- Consider problem complexity

### Debug Mode

Enable detailed logging:
```bash
export TREEWIDZARD_DEBUG=1
./treewidzard <options>
```

### Getting Help

1. Check command line help: `./treewidzard --help`
2. Review example files in `system-paper/` directory
3. Examine test cases in `test_conjectures/`
4. Consult architecture documentation in `docs/`

## Examples Repository

See the `system-paper/` directory for complete examples:
- `ATP/`: Automated theorem proving examples
- `model-checking-option-1/`: Basic model checking
- `model-checking-option-2/`: Advanced model checking
- `invariant-computation/`: Graph invariant examples

Each directory contains:
- Input files (graphs, decompositions, conjectures)
- Command examples
- Expected outputs
