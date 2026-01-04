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

## Installation (Build from Source)


All commands below should be executed in a Linux terminal. For instance, in Ubuntu to open a terminal click in ``activities`` and then type ``terminal``.

### Requirements
The following software are assumed to be installed on your machine:

1- A C++20 compiler (tested with GCC/11.2.0, should also work with any later version)

2- CMake/3.0.0 or a later version

### First Installation

Clone from GitHub:
```commandline
git clone https://github.com/AutoProving/TreeWidzard-Engine
```

Move into the TreeWidzard directory:
```commandline
cd TreeWidzard-Engine
```

Configure build directory using CMake (here with the build directory named `Build`):
```commandline
cmake -BBuild
```

Compile the program:
```commandline
cmake --build Build
```

### Updating
Download any changes to the bundling code itself:
```commandline
git pull
```

For recompiling, run this again:
```commandline
cmake --build Build
```

## Running TreeWidzard
To print usage:
```
./Build/treewidzard --help
```

## Testing

See `docs/TESTING.md`.

## 📚 Using as a Library

TreeWidzard can now be linked as a shared or static library in your own C++ projects.

### C++ API (`libtreewidzard`)
Link against `TreeWidzard-Core` and include the public header:

```cpp
#include <treewidzard/TreeWidzard.h>

int main() {
    TreeWidzard::Engine engine;
    
    // 1. Load a dynamic core (plugin)
    engine.loadCore("ChromaticNumber", 0);
    
    // 2. Set Width
    engine.setWidth(5);
    
    // 3. Define Conjecture
    engine.setConjecture("x = ChromaticNumber");
    
    // 4. Solve
    bool result = engine.solve();
    return 0;
}
```

### 🐍 Python Bindings

TreeWidzard includes Python bindings via `pybind11`.

#### Installation
The bindings are built automatically when `BUILD_PYTHON_BINDINGS=ON` (default).
To install them into your Python environment:

```bash
cd Build
sudo make install
```

#### Usage
```python
import treewidzard

# Initialize Engine
engine = treewidzard.Engine()

# Configure
engine.loadCore("ChromaticNumber", 0)
engine.setWidth(10)
engine.setConjecture("x < 10")

# Run
if engine.solve():
    print("Property satisfied!")
else:
    print("Property not satisfied.")
```
