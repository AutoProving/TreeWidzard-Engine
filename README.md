# TreeWidzard

## Installation

All commands below should be executed in a Linux terminal. For instance, in Ubuntu to open a terminal click in ``activities`` and then type ``terminal``.

### Requirements
The following software are assumed to be installed on your machine:

1- A C++20 compiler (tested with GCC/11.2.0, should also work with any later version)

2- CMake/3.18 or a later version

### First Installation

Clone from GitHub:
```commandline
git clone https://github.com/AutoProving/TreeWidzard-Engine
```

Move into the TreeWidzard directory:
```commandline
cd TreeWidzard-Engine
```

Configure the build directory under `out/`, which is ignored by Git:
```commandline
cmake -S . -B out/build/default -DCMAKE_BUILD_TYPE=Release
```

Compile the program:
```commandline
cmake --build out/build/default
```

### Updating
Download any changes to the bundling code itself:
```commandline
git pull
```

For recompiling, run this again:
```commandline
cmake --build out/build/default
```

## Running TreeWidzard
To print usage:
```
./out/build/default/treewidzard --help
```
