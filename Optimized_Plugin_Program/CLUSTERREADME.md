# Executing TreeWidzard on Saga

Checking which modules are available on the cluster:

````module avail````

1- Load GCC/11.2.0

```module load GCC/11.2.0```

2- Load CMake/3.12.1

```module load CMake/3.12.1```

3- Upload TreeWidzard's zip file to the cluster

```commandline
scp TreeWidzard.zip <username>@saga.sigma2.no:
```
Observation: Keep ``:`` after ``saga.sigma2.no``.

4- Unzip TreeWidzard's zip file

````commandline
unzip TreeWidzard.zip
````

5- Enter the directory of TreeWidzard

```commandline
cd TreeWidzard
```

6- Create a directory called ``Build``

NOTE: If the directory exists, remove all content of the directory

7- Enter ``Build``

```commandline
cd Build
```

8- Run ``cmake`` in ``Build``

```cmake ..```

9- Run ``make`` in ``Build``

```make```

The program ready to use.

For Test:

In ``Build`` folder, run the following command
```commandline
./treewidzard -atp pw = 3 -pl -nthreads 3 ParallelBreadthFirstSearch ../small-tests/conjecture.txt
```

This command will test TreeWidzard on a conjecture on graphs of pathwidth at most 3 and using 3 threads. 

### Running Experiments

Make sure **treewidzard** is working.

Let **PROGRAM_FILE** be **TreeWidzard**'s executable file.

Let **CLUSTER_OUTPUT_PATH** be the path that you want to store the cluster outputs.

Let **OUTPUT** be the output file of TreeWidzard.

Let **CONJECTURE_FILE** be the conjecture file.

Let **SEARCHSTRATEGY** be the search strategy.

Example:
### normal JOB
```commandline
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
#SBATCH --account=nn9535k --job-name=MyJob
#SBATCH --partition=bigmem
#SBATCH --time=1-0:0:0
#SBATCH --ntasks=1 --cpus-per-task=32
#SBATCH --mem-per-cpu=32G
#SBATCH --output=/CLUSTER_OUTPUT_PATH/slurm_%x_%j.out
#SBATCH --error=/CLUSTER_OUTPUT_PATH/slurm_%x_%j.err
PROGRAM_FILE -atp pw = 3 -pl -nthreads 60 SEARCHSTRATEGY CONJECTURE_FILE > OUTPUT
exit 0
```

