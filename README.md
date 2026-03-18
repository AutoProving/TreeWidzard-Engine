# TreeWidzard

## Quick Start

For most users, the simplest path is a release archive.

1. Download the archive for your platform from the repository's GitHub Releases page.
2. Unpack it.
3. Run the launcher in `treewidzard/bin/`.

Launcher paths:

- Linux/macOS: `treewidzard/bin/treewidzard`
- Windows: `treewidzard\\bin\\treewidzard.bat`

## Installation (Build from Source)

Building from source is mainly for developers.

1. Clone the repository:

```bash
git clone https://github.com/farhad-vadiee/TreeWidzard.git
cd TreeWidzard
```

2. Check whether the current machine already has the required build tools.

On Linux or macOS:

```bash
./check_requirements.sh
```

On Windows:

```bat
check_requirements.bat
```

3. If the checker passes:

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

4. If the checker fails, install the missing requirements for your system and run the checker again.

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

The checked-in parser sources are used by default, so most source builds do not require Bison or Flex.

### Optional: Regenerate the Parser

The repository already includes generated parser files:

- `Parser/PropertyParser/input_parser.cpp`
- `Parser/PropertyParser/input_parser.hpp`
- `Parser/PropertyParser/input_lexer.cpp`

You only need Bison 3.0+ and Flex 2.6+ if you edit:

- `Parser/PropertyParser/input_parser.y`
- `Parser/PropertyParser/input_lexer.l`

To regenerate those files during configuration:

```bash
cmake -S . -B build -DTREEWIDZARD_REGENERATE_PROPERTY_PARSER=ON
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
