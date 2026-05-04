This directory contains a runnable reconstruction of the Reed chapter experiment
inventory using the current TreeWidzard core names.

Sources:

- `../thesis/thesis-mono/chapters/sections/Reed-conjecture.tex`
- `../state-canonizing/fullVersion/08-New-Experiments.tex`

Current core-name mapping:

- `HasMultipleEdges()`
- `SimpleCliqueNumber_AtLeast(3)`
- `MaximumDegree_AtLeast(d)`
- `ChromaticNumber_AtMost(c)`

Included experiment groups:

- `pathwidth-ablation`: 36 ATP runs
- `treewidth-ablation`: 18 ATP runs
- `largest-confirmation`: 1 ATP run
- `counterexample-atp`: 3 ATP runs
- `counterexample-recheck`: 3 model-checking reruns

Total runnable steps: 61

Notes:

- The main chapter tables are reproduced as run matrices over width, degree bound,
  and search configuration.
- For `-premise` runs, the full subgraph-closed guard is encoded as the single
  left-hand antecedent of the implication, matching the premise-search chapter.
- The `pw=5, Delta=3` run is mapped to the current repo's combined parallel,
  canonized configuration:
  `ParallelIsomorphismBreadthFirstSearch + -premise + -nthreads 128`.
- The chapter prose names the extra counterexample workflows, but not always the
  exact command line; those runs are reconstructed here from the stated claims.

Useful commands:

```bash
python3 temp-reed-test/chapter-runs/run_reed_chapter.py --summary
python3 temp-reed-test/chapter-runs/run_reed_chapter.py --list
python3 temp-reed-test/chapter-runs/run_reed_chapter.py --group pathwidth-ablation --run
python3 temp-reed-test/chapter-runs/run_reed_chapter.py --group treewidth-ablation --run
python3 temp-reed-test/chapter-runs/run_reed_chapter.py --group largest-confirmation --run
python3 temp-reed-test/chapter-runs/run_reed_chapter.py --group counterexample-atp --run
python3 temp-reed-test/chapter-runs/run_reed_chapter.py --group counterexample-recheck --run
```

Outputs are written under `temp-reed-test/chapter-runs/runs/<workspace-id>/`.
