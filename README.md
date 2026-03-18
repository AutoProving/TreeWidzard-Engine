# TreeWidzard

## 🐳 Quick Start (Docker)

The easiest way to run TreeWidzard is using Docker. This avoids setting up C++ compilers and dependencies manually.

### Prerequisites
- [Docker](https://docs.docker.com/get-docker/) installed.

### Running TreeWidzard
To run the tool on files in your current directory:

```bash
# Pull and run the latest version
docker run --rm -v $(pwd):/data ghcr.io/farhad-vadiee/treewidzard:latest --help
```

To analyze files (e.g., a graph and property file in your current folder):
```bash
docker run --rm -v $(pwd):/data ghcr.io/farhad-vadiee/treewidzard:latest \
    -modelcheck PACE /data/property.txt /data/graph.gr
```

### Building Locally (Optional)
If you prefer to build the image yourself:
```bash
docker build -t treewidzard:latest .
docker run --rm -v $(pwd):/data treewidzard:latest --help
```

## Recommended For Non-Specialists

If you do not want to install compilers or build tools, use one of these:

1. Docker: run `ghcr.io/farhad-vadiee/treewidzard:latest`
2. GitHub Releases: download the archive for your platform, unpack it, and run the launcher in `treewidzard/bin/`

When release archives are published, the launcher paths are:

- Linux/macOS: `treewidzard/bin/treewidzard`
- Windows: `treewidzard\\bin\\treewidzard.bat`

## Installation (Build from Source)

Building from source is mainly for developers. TreeWidzard can be built from the command line on Linux, macOS, and Windows.

### Requirements

- A C++20 compiler
- CMake 3.18 or newer
- Bison 3.0 or newer
- Flex 2.6 or newer

### Install Prerequisites

#### Linux (Ubuntu/Debian)

```bash
sudo apt-get update
sudo apt-get install -y git cmake build-essential bison flex
```

#### macOS (Homebrew)

```bash
brew install git cmake bison flex
```

#### Windows (MSYS2 MinGW 64-bit)

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
  mingw-w64-x86_64-ninja \
  mingw-w64-x86_64-bison \
  mingw-w64-x86_64-flex
```

### Build In Place

Use your repository URL or fork URL:

```bash
git clone https://github.com/farhad-vadiee/TreeWidzard.git
cd TreeWidzard
```

On Linux or macOS:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
./treewidzard.sh --help
```

On Windows, run these from the `MSYS2 MinGW 64-bit` terminal:

```bash
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
./treewidzard.bat --help
```

### Install To a Self-Contained Prefix

On Linux or macOS:

```bash
cmake -S . -B build-install -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX="$PWD/install"
cmake --build build-install -j
cmake --install build-install
./install/bin/treewidzard --help
```

On Windows, run these from the `MSYS2 MinGW 64-bit` terminal:

```bash
cmake -S . -B build-install -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX="$PWD/install"
cmake --build build-install -j
cmake --install build-install
./install/bin/treewidzard.bat --help
```

### Updating

Download new changes:

```bash
git pull
```

Rebuild:

```bash
cmake --build build
```

## Running TreeWidzard

To print usage from the build tree on Linux or macOS:

```bash
./treewidzard.sh --help
```

To print usage from the build tree on Windows:

```bash
./treewidzard.bat --help
```

To run the installed binary from the optional install prefix on Linux or macOS:

```bash
./install/bin/treewidzard --help
```

To run the installed binary from the optional install prefix on Windows:

```bash
./install/bin/treewidzard.bat --help
```

## Minimal Conjecture Example

Example conjecture file:

```text
x := MaxDegree(4)
y := ChromaticNumber(4)
Formula
x IMPLIES y
```

Example ATP run from the build tree on Linux or macOS:

```bash
./treewidzard.sh -atp tw=2 BreadthFirstSearch examples/conjectures/simple_implies.txt
```

On Windows:

```bash
./treewidzard.bat -atp tw=2 BreadthFirstSearch examples/conjectures/simple_implies.txt
```

## Testing

Run the test suite from the repo root:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
ctest --test-dir build --output-on-failure -j
```

Run only certificate tests:

```bash
ctest --test-dir build --output-on-failure -R '^Certificates\\.'
```

## 📚 Using as a Library

TreeWidzard can now be linked as a shared or static library in your own C++ projects.

### C++ API (`libtreewidzard`)
Link against `TreeWidzard-Core` and include the public header:

```cpp
#include <treewidzard/TreeWidzard.h>

int main() {
    TreeWidzard::Engine engine;
    
    // 1. Load a dynamic core (plugin)
    engine.loadCore("ChromaticNumber", 3);
    
    // 2. Set Width
    engine.setWidth(2);
    
    // 3. Define Conjecture over the loaded core(s)
    engine.setConjecture("x");
    
    // 4. Solve
    bool result = engine.solve();
    return 0;
}
```
