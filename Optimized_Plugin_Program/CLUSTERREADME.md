Steps to run treewidzard

Command for see all  available modules in the cluster:

````module avail````

1- Load GCC/11.2.0

```module load GCC/11.2.0```

2- Load CMake/3.12.1

```module load CMake/3.12.1```

3- Create a directory named ``Build``

NOTE: If the directory exists, remove all content of the directory

4- Run ``cmake`` in ``Build``

```cmake ..```

5- Run ``make`` in ``Build``

```make```

The program ready to use.

For Test:

In ``Build`` folder, run the following command
```commandline
./treewidzard -atp pw = 3 -pl -nthreads 3 ParallelBreadthFirstSearch ../small-tests/conjecture.txt
```

### Running Experiments

Make sure **treewidzard** is working.

Let **PROGRAM_FILE** be **treewidzard**'s file.

Let **CLUSTER_OUTPUT_PATH** be the path that you want to store the cluster output.

Let **OUTPUT** be the put file of the program.

Let **CONJECTURE_FILE** be the conjecture file.

Let **SEARCHSTRATEGY** be the search strategy.

Example:
### normal JOB
```commandline
#!/bin/bash
#SBATCH --account=nn9535k --job-name=MyJob
#SBATCH --partition=normal
#SBATCH --time=0-00:10:00
#SBATCH --ntasks=1 --cpus-per-task=32
#SBATCH --mem-per-cpu=3G
#SBATCH --output=/CLUSTER_OUTPUT_PATH/slurm_%x_%j.out
#SBATCH --error=/CLUSTER_OUTPUT_PATH/slurm_%x_%j.err
PROGRAM_FILE -atp pw = 3 -pl -nthreads 3 SEARCHSTRATEGY CONJECTURE_FILE > OUTPUT
exit 0
```

### bigmem JOB
```commandline
#SBATCH --account=MyProject --job-name=MyJob
#SBATCH --partition=bigmem
#SBATCH --time=1-0:0:0
#SBATCH --ntasks=2 --cpus-per-task=4
#SBATCH --mem-per-cpu=32G
#SBATCH --output=/CLUSTER_OUTPUT_PATH/slurm_%x_%j.out
#SBATCH --error=/CLUSTER_OUTPUT_PATH/slurm_%x_%j.err
PROGRAM_FILE -atp pw = 3 -pl -nthreads 3 SEARCHSTRATEGY CONJECTURE_FILE > OUTPUT
exit 0
```

