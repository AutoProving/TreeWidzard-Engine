Step to run treewidzard

Command for see all  available modules in the cluster:

````module avail````

1- Load GCC/11.2.0

```module load GCC/11.2.0```

2- Load CMake/3.12.1

```module load CMake/3.12.1```

3- Create a directory named ``Build``

NOTE: If the directory exists, remove all content of the directory

4- Run cmake in ``Build``

```cmake ..```

5- Run make in ``Build``

```make```

The program ready to use.