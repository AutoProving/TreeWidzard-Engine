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
mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make
```

On Windows, run these from the `MSYS2 MinGW 64-bit` terminal:

```bash
mkdir build
cd build
cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Release
ninja
```

You do not need this command unless you want to regenerate the property parser. In that case, use:

```bash
cmake -S . -B build -DTREEWIDZARD_REGENERATE_PROPERTY_PARSER=ON
```

This requires Bison 3.0+ and Flex 2.6+.

## Running TreeWidzard

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
- `<property file>`: a text file that defines the DP cores and the formula/conjecture to check. Example: [examples/system_paper/atp/five_colorable.txt](/Users/farhad/Documents/uib/github/TreeWidzard/examples/system_paper/atp/five_colorable.txt)

Available search methods:

- `BreadthFirstSearch`
- `IsomorphismBreadthFirstSearch`
- `ParallelBreadthFirstSearch`
- `ParallelIsomorphismBreadthFirstSearch`

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

- `<property file>`: a text file that defines the DP cores and the formula/conjecture to check. Example: [examples/system_paper/model_checking/three_colorable.txt](/Users/farhad/Documents/uib/github/TreeWidzard/examples/system_paper/model_checking/three_colorable.txt)
- `<graph file>`: an input graph in PACE graph format. Example: [examples/system_paper/model_checking/diamond.gr](/Users/farhad/Documents/uib/github/TreeWidzard/examples/system_paper/model_checking/diamond.gr)
- `<tree decomposition file>`: a tree decomposition of the input graph in PACE decomposition format. Example: [examples/system_paper/model_checking/diamond.td](/Users/farhad/Documents/uib/github/TreeWidzard/examples/system_paper/model_checking/diamond.td)

ITD format:

```bash
./treewidzard.sh -modelcheck ITD [flags] <property file> <instructive tree decomposition file>
```

Example:

```bash
./treewidzard.sh -modelcheck ITD examples/system_paper/model_checking/three_colorable.txt examples/system_paper/model_checking/diamond.itd
```

Terms:

- `<property file>`: a text file that defines the DP cores and the formula/conjecture to check. Example: [examples/system_paper/model_checking/three_colorable.txt](/Users/farhad/Documents/uib/github/TreeWidzard/examples/system_paper/model_checking/three_colorable.txt)
- `<instructive tree decomposition file>`: an instructive tree decomposition input used by TreeWidzard for model checking. Example: [examples/system_paper/model_checking/diamond.itd](/Users/farhad/Documents/uib/github/TreeWidzard/examples/system_paper/model_checking/diamond.itd)

## Flags

| Flag | ATP | Model checking | Meaning |
| --- | --- | --- | --- |
| `-pl` | Yes | No | Print loop progress |
| `-ps` | Yes | No | Print discovered states |
| `-st` | Yes | No | Print the state tree for BreadthFirstSearch variants |
| `-nthreads N` | Yes | No | Set the thread count for `ParallelBreadthFirstSearch` and `ParallelIsomorphismBreadthFirstSearch` |
| `-files` | Yes | Yes | Write generated artifacts to files |

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
