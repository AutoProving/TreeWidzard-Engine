# TreeWidzard

## Installation

All commands below should be executed in a Linux terminal. For instance, in Ubuntu to open a terminal click in ``activities`` and then type ``terminal``.

### Requirements
The following software are assumed to be installed on your machine:

1- A C++20 compiler (tested with GCC/11.2.0, should also work with any later version)

2- CMake/3.0.0 or a later version

### First Installation

Clone from GitHub:
```commandline
git clone https://github.com/AutoProving/TreeWidzard-Release
```

Move into the TreeWidzard directory:
```commandline
cd TreeWidzard-Release
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
