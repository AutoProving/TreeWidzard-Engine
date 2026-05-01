# TreeWidzard

TreeWidzard is a C++ tool for model checking, computing graph invariants, and automated theorem proving on bounded treewidth or pathwidth graph classes. It loads dynamic DP cores and search strategies, then searches for counterexamples to conjectures or verifies properties on tree decompositions.

## Requirements

To build TreeWidzard from source, you need:

- CMake 3.18 or newer
- A C++20 compiler
- A build tool such as `make` or `ninja`

Optional:

- Git for cloning or updating the repository
- Bison 3.0+ and Flex 2.6+ if you want to regenerate the property parser

You can check whether your machine already has the required tools by running:

- Linux/macOS: `./check_requirements.sh`
- Windows: `check_requirements.bat`

## Installation (Build from Source)

Building from source is mainly for developers.

1. Download the source code or clone the repository.

Download the repository as an archive from GitHub, or clone it with:

```bash
git clone https://github.com/farhad-vadiee/TreeWidzard.git
cd TreeWidzard
```

2. Check whether the current machine already has the required build tools. If something is missing, go to [Install Requirements](#install-requirements), install the missing tools for your platform, and then run the checker again.

On Linux or macOS:

```bash
./check_requirements.sh
```

On Windows:

```bat
check_requirements.bat
```

3. Build the project.

On Linux or macOS:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTS=OFF
cmake --build build -j 8
```

On Windows, run these from the `MSYS2 MinGW 64-bit` terminal:

```bash
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTS=OFF
cmake --build build
```

The tests are not needed for normal use. If you are developing TreeWidzard and want to run the test suite, configure with `-DBUILD_TESTS=ON` and then run `ctest --test-dir build --output-on-failure`.

You do not need this command unless you want to regenerate the property parser. In that case, use:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTS=OFF -DTREEWIDZARD_REGENERATE_PROPERTY_PARSER=ON
```

This requires Bison 3.0+ and Flex 2.6+.

## Running TreeWidzard

The main program should be executed from the repository root. Run the commands below from the root folder, not from the `build/` directory. The wrapper scripts look for the compiled executable under `build/`, so `build/` must exist and be up to date, but it should not be your working directory when using paths such as `examples/...`.

To check that the installation worked:

Linux/macOS:

```bash
./treewidzard.sh --help
```

Windows:

```bat
./treewidzard.bat --help
```

To run a search on one of the example conjectures:

Linux/macOS:

```bash
./treewidzard.sh -atp tw=4 -pl ParallelBreadthFirstSearch examples/system_paper/atp/five_colorable.txt
```

Windows:

```bat
./treewidzard.bat -atp tw=4 -pl ParallelBreadthFirstSearch examples/system_paper/atp/five_colorable.txt
```

For a compact presentation script with correct and incorrect ATP and model-checking commands, see [examples.md](examples.md).

## ATP Command Structure

ATP command pattern:

```bash
./treewidzard.sh -atp <width type>=<width value> [flags] <search method> <property file>
```

Example:

```bash
./treewidzard.sh -atp tw=4 -pl ParallelBreadthFirstSearch examples/system_paper/atp/five_colorable.txt
```

Terms:

- `<search method>`: the search strategy used to explore graphs, such as `BreadthFirstSearch` or `ParallelBreadthFirstSearch`
- `<width type>`: the width parameter to use, such as `tw` or `pw`
- `<width value>`: the width bound to test
- `<property file>`: a text file that defines the DP cores and the formula/conjecture to check. Example: [examples/system_paper/atp/five_colorable.txt](examples/system_paper/atp/five_colorable.txt)

Available search methods:

- `BreadthFirstSearch`
- `IsomorphismBreadthFirstSearch`
- `ParallelBreadthFirstSearch`
- `ParallelIsomorphismBreadthFirstSearch`

## ATP Examples: Symmetry Breaking and Premise Search

The examples below use [examples/conjectures/simple_implies.txt](examples/conjectures/simple_implies.txt), which states that graphs with maximum degree at least 4 are 4-colorable. The `-pl` flag prints loop progress; the state counts below are the final reported counts from those runs.

Baseline ATP:

```bash
./treewidzard.sh -atp tw=2 -pl BreadthFirstSearch examples/conjectures/simple_implies.txt
```

This run reports 340 states and satisfies the property.

### ATP with Symmetry Breaking

Use an isomorphism-aware search method:

```bash
./treewidzard.sh -atp tw=2 -pl IsomorphismBreadthFirstSearch examples/conjectures/simple_implies.txt
```

For this example, symmetry breaking reduces the reported state count from 340 to 89.

### ATP with Premise Search

Use `-premise` for an implication whose premise is subgraph-closed:

```bash
./treewidzard.sh -atp tw=2 -pl -premise BreadthFirstSearch examples/conjectures/simple_implies.txt
```

For this example, premise search reduces the reported state count from 340 to 1.

### ATP with Symmetry Breaking and Premise Search

The two optimizations can be combined:

```bash
./treewidzard.sh -atp tw=2 -pl -premise IsomorphismBreadthFirstSearch examples/conjectures/simple_implies.txt
```

For this example, the combined run also reports 1 state.

## Model Checking Command Structure

PACE format:

```bash
./treewidzard.sh -modelcheck PACE [flags] <property file> <graph file> <tree decomposition file>
```

Example:

```bash
./treewidzard.sh -modelcheck PACE examples/system_paper/model_checking/three_colorable.txt examples/system_paper/model_checking/diamond.gr examples/system_paper/model_checking/diamond.td
```

Terms:

- `<property file>`: a text file that defines the DP cores and the formula/conjecture to check. Example: [examples/system_paper/model_checking/three_colorable.txt](examples/system_paper/model_checking/three_colorable.txt)
- `<graph file>`: an input graph in PACE graph format. Example: [examples/system_paper/model_checking/diamond.gr](examples/system_paper/model_checking/diamond.gr)
- `<tree decomposition file>`: a tree decomposition of the input graph in PACE decomposition format. Example: [examples/system_paper/model_checking/diamond.td](examples/system_paper/model_checking/diamond.td)

ITD format:

```bash
./treewidzard.sh -modelcheck ITD [flags] <property file> <instructive tree decomposition file>
```

Example:

```bash
./treewidzard.sh -modelcheck ITD examples/system_paper/model_checking/three_colorable.txt examples/system_paper/model_checking/diamond.itd
```

Terms:

- `<property file>`: a text file that defines the DP cores and the formula/conjecture to check. Example: [examples/system_paper/model_checking/three_colorable.txt](examples/system_paper/model_checking/three_colorable.txt)
- `<instructive tree decomposition file>`: an instructive tree decomposition input used by TreeWidzard for model checking. Example: [examples/system_paper/model_checking/diamond.itd](examples/system_paper/model_checking/diamond.itd)

## Flags

| Flag | ATP | Model checking | Meaning |
| --- | --- | --- | --- |
| `-pl` | Yes | No | Print loop progress |
| `-ps` | Yes | No | Print discovered states |
| `-st` | Yes | No | Print the state tree for BreadthFirstSearch variants |
| `-premise` | Yes | No | Enable premise pruning. Use this only for properties of the form `A IMPLIES B`, where `A` is subgraph-closed |
| `-pdbgn` | Yes | Yes | Write the graph in directed bipartite NAUTY format |
| `-no-bfs-dag` | Yes | No | Disable BFS DAG construction during search. This can reduce overhead, but may prevent full counterexample reconstruction |
| `-nthreads N` | Yes | No | Set the thread count for `ParallelBreadthFirstSearch` and `ParallelIsomorphismBreadthFirstSearch` |
| `-files` | Yes | Yes | Write generated artifacts to files |

## Supported Formula Operations

| Kind | Supported operations |
| --- | --- |
| Boolean operators | `AND`, `OR`, `NOT`, `IMPLIES`, `IFF` |
| Comparisons | `==`, `>=`, `<=`, `>`, `<` |
| Arithmetic operators | `+`, `-`, `*`, `/` |
| Binary functions | `max(a, b)`, `min(a, b)`, `pow(a, b)` |
| Unary functions | `abs(x)`, `acos(x)`, `asin(x)`, `atan(x)`, `cos(x)`, `exp(x)`, `floor(x)`, `ln(x)`, `log(x)`, `sin(x)`, `sqrt(x)`, `tan(x)` |
| Invariants | `INV(x)` where `x` is a DP-core variable |

## Variables and Invariants

A variable such as `x` is Boolean. It records whether the current graph is accepted by the DP-core assigned to `x`.

```text
x := ChromaticNumber_AtMost(3)
Formula
x
```

In this example, `x` is true exactly when the graph is accepted by the 3-colorability DP-core.

For DP-cores computing invariants, the variable is still Boolean, while `INV(x)` is the actual invariant value computed by the core.

```text
x := IndependenceNumber()
Formula
INV(x) >= 2
```

In this example, `INV(x)` is the independence number. For Boolean DP-cores, `INV(x)` is equal to `x` and is reported as `0` or `1`.

## Available DP-Cores

### Boolean DP-Cores

| DP-core | Name in property file | Parameter type | Example |
| --- | --- | --- | --- |
| Chromatic number at most | `ChromaticNumber_AtMost` | Unsigned integer | `x := ChromaticNumber_AtMost(3)` |
| Clique number at least | `CliqueNumber_AtLeast` | Unsigned integer | `x := CliqueNumber_AtLeast(4)` |
| Clique number at least, simple graphs | `SimpleCliqueNumber_AtLeast` | Unsigned integer | `x := SimpleCliqueNumber_AtLeast(4)` |
| Feedback vertex set number at most | `FeedbackVertexSetNumber_AtMost` | Unsigned integer | `x := FeedbackVertexSetNumber_AtMost(2)` |
| Hamiltonian cycle | `HasHamiltonianCycle` | None | `x := HasHamiltonianCycle()` |
| Hamiltonian path | `HasHamiltonianPath` | None | `x := HasHamiltonianPath()` |
| K2 minor containment | `ContainsK2Minor` | None | `x := ContainsK2Minor()` |
| Minor containment | `ContainsMinor` | Multi-parameter | `x := ContainsMinor("0 1 1 2")` |
| Multiple-edge detection | `HasMultipleEdges` | None | `x := HasMultipleEdges()` |
| Maximum degree at least | `MaximumDegree_AtLeast` | Unsigned integer | `x := MaximumDegree_AtLeast(4)` |
| Perfect matching | `HasPerfectMatching` | None | `x := HasPerfectMatching()` |
| Vertex cover number at most | `VertexCoverNumber_AtMost` | Unsigned integer | `x := VertexCoverNumber_AtMost(2)` |

### DP-Cores Computing Invariants

| DP-core | Name in property file | Invariant | Example |
| --- | --- | --- | --- |
| Independence number | `IndependenceNumber` | Maximum independent set size | `x := IndependenceNumber()` |
| Vertex count | `VertexCount` | Number of vertices | `x := VertexCount()` |

Example property file:

```text
x := ChromaticNumber_AtMost(3)
y := IndependenceNumber()
Formula
x AND (INV(y) >= 2)
```

## Install Requirements

Linux (Ubuntu/Debian):

```bash
sudo apt-get update
sudo apt-get install -y git cmake build-essential
```

macOS (Homebrew):

```bash
xcode-select --install
brew install git cmake
```

Windows (MSYS2 MinGW 64-bit):

Install [MSYS2](https://www.msys2.org/), open the `MSYS2 MinGW 64-bit` terminal, then run:

```bash
pacman -Syu
```

Close that terminal, reopen `MSYS2 MinGW 64-bit`, then run:

```bash
pacman -S --needed \
  git \
  mingw-w64-x86_64-toolchain \
  mingw-w64-x86_64-cmake \
  mingw-w64-x86_64-ninja
```

## License

TreeWidzard is released under the MIT License. See [LICENSE](LICENSE) for details.
