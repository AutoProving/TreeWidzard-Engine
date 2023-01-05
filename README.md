# TreeWidzard Installation

## Installing TreeWidzard in a machine running Linux

All commands below should be executed in a Linux terminal. For instance, in Ubuntu to open a terminal click in ``activities`` and then type ``terminal``.

#### Requirements
The following software are assumed to be installed on your machine:

1- GCC/11.2.0 or a later version

2- CMake/3.12.1 or a later version

#### Downloading TreeWidzard

There are two main ways of downloading TreeWidzard.

1- Cloning from GitHub:
```commandline
git clone https://github.com/AutoProving/treewidzard.git
```

2- You can also download the zip file of TreeWidzard's repository.

Download the zip file:
```commandline
wget https://github.com/AutoProving/treewidzard/archive/refs/heads/main.zip TreeWizard
```

Extract the files from the zip file:
```commandline
unzip TreeWidzard-main.zip 
```

Rename the directory to TreeWidzard

```commandline
mv TreeWidzard-main TreeWidzard
```

#### Compiling TreeWidzard

1- Enter the directory of TreeWidzard

```commandline
cd TreeWidzard
```

2- Create a folder called ``Build``. This is the folder where the files will be compiled and where TreeWidzard's executable file will be created.

```commandline
mkdir Build
```

3- Enter the ``Build`` folder

```commandline
cd Build
```

4- Run ``cmake``

```commandline
cmake ..
```

5- Run ``make``. This command creates TreeWidzard's executable file.

```commandline
make
```

5*- Run ``cmake --build . -- -j 8  `` to compile in parallel and fast.

#### Executing TreeWidzard

Execute ``./treewidzard --help`` to obtain a list of instructions on how to use TreeWidzard to model check properties or to test conjectures.


