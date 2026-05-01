# TreeWidzard Presentation Examples

These commands are intended for a short live demo from the repository root.

Build first if needed:

```bash
cmake --build build -j 8
```

## Automated theorem proving

Correct statement: every graph of treewidth at most 4 is 5-colorable.

```bash
./treewidzard.sh -atp tw=4 -pl ParallelBreadthFirstSearch \
  examples/system_paper/atp/five_colorable.txt
```

Expected key line:

```text
Result: PROPERTY SATISFIED
```

Incorrect statement: every graph of treewidth at most 4 is 4-colorable.

```bash
./treewidzard.sh -atp tw=4 -pl ParallelBreadthFirstSearch \
  examples/system_paper/atp/four_colorable.txt
```

Expected key lines:

```text
Result: PROPERTY NOT SATISFIED
Counterexample found
```

## Model checking

Correct concrete-graph statement: the diamond graph is 3-colorable.

```bash
./treewidzard.sh -modelcheck PACE \
  examples/presentation/model_checking/three_colorable.txt \
  examples/presentation/model_checking/diamond.gr \
  examples/presentation/model_checking/diamond.td
```

Expected key line:

```text
Result:PROPERTY SATISFIED
```

Incorrect concrete-graph statement: the diamond graph is 2-colorable.

```bash
./treewidzard.sh -modelcheck PACE \
  examples/presentation/model_checking/two_colorable.txt \
  examples/presentation/model_checking/diamond.gr \
  examples/presentation/model_checking/diamond.td
```

Expected key line:

```text
Result:PROPERTY NOT SATISFIED
```

Equivalent ITD input command:

```bash
./treewidzard.sh -modelcheck ITD \
  examples/presentation/model_checking/three_colorable.txt \
  examples/presentation/model_checking/diamond.itd
```

Expected key line:

```text
Result:PROPERTY SATISFIED
```

## Invariants

Correct invariant statement: the diamond graph has independence number 2.

```bash
./treewidzard.sh -modelcheck PACE \
  examples/presentation/model_checking/independence_number_2.txt \
  examples/presentation/model_checking/diamond.gr \
  examples/presentation/model_checking/diamond.td
```

Expected key lines:

```text
Result:PROPERTY SATISFIED
Invariant value: INV(x)=2
```

Incorrect invariant statement: the diamond graph has independence number 3.

```bash
./treewidzard.sh -modelcheck PACE \
  examples/presentation/model_checking/independence_number_3.txt \
  examples/presentation/model_checking/diamond.gr \
  examples/presentation/model_checking/diamond.td
```

Expected key lines:

```text
Result:PROPERTY NOT SATISFIED
Invariant value: INV(x)=2
```

## More quick examples

These model-checking examples all use the same diamond graph and tree decomposition:

```bash
GRAPH=examples/presentation/model_checking/diamond.gr
TD=examples/presentation/model_checking/diamond.td
ITD=examples/presentation/model_checking/diamond.itd
```

Triangle exists:

```bash
./treewidzard.sh -modelcheck PACE examples/presentation/model_checking/triangle.txt "$GRAPH" "$TD"
```

Expected:

```text
Result:PROPERTY SATISFIED
```

No 4-clique exists:

```bash
./treewidzard.sh -modelcheck PACE examples/presentation/model_checking/no_four_clique.txt "$GRAPH" "$TD"
```

Expected:

```text
Result:PROPERTY NOT SATISFIED
```

Maximum degree is at least 3:

```bash
./treewidzard.sh -modelcheck PACE examples/presentation/model_checking/max_degree_at_least_3.txt "$GRAPH" "$TD"
```

Expected:

```text
Result:PROPERTY SATISFIED
```

Maximum degree is not at least 4:

```bash
./treewidzard.sh -modelcheck PACE examples/presentation/model_checking/max_degree_at_least_4.txt "$GRAPH" "$TD"
```

Expected:

```text
Result:PROPERTY NOT SATISFIED
```

Vertex cover number is at most 2:

```bash
./treewidzard.sh -modelcheck PACE examples/presentation/model_checking/vertex_cover_at_most_2.txt "$GRAPH" "$TD"
```

Expected:

```text
Result:PROPERTY SATISFIED
```

Vertex cover number is not at most 1:

```bash
./treewidzard.sh -modelcheck PACE examples/presentation/model_checking/vertex_cover_at_most_1.txt "$GRAPH" "$TD"
```

Expected:

```text
Result:PROPERTY NOT SATISFIED
```

Perfect matching exists:

```bash
./treewidzard.sh -modelcheck PACE examples/presentation/model_checking/perfect_matching.txt "$GRAPH" "$TD"
```

Expected:

```text
Result:PROPERTY SATISFIED
```

Hamiltonian path exists:

```bash
./treewidzard.sh -modelcheck PACE examples/presentation/model_checking/hamiltonian_path.txt "$GRAPH" "$TD"
```

Expected:

```text
Result:PROPERTY SATISFIED
```

Hamiltonian cycle exists:

```bash
./treewidzard.sh -modelcheck PACE examples/presentation/model_checking/hamiltonian_cycle.txt "$GRAPH" "$TD"
```

Expected:

```text
Result:PROPERTY SATISFIED
```

The graph has no multiple edges:

```bash
./treewidzard.sh -modelcheck PACE examples/presentation/model_checking/no_multiple_edges.txt "$GRAPH" "$TD"
```

Expected:

```text
Result:PROPERTY SATISFIED
```

## More graph inputs

These examples use a path on four vertices:

```bash
PATH_GRAPH=examples/presentation/model_checking/path4.gr
PATH_TD=examples/presentation/model_checking/path4.td
```

The path is 2-colorable:

```bash
./treewidzard.sh -modelcheck PACE examples/presentation/model_checking/two_colorable.txt "$PATH_GRAPH" "$PATH_TD"
```

Expected:

```text
Result:PROPERTY SATISFIED
```

The path has no triangle:

```bash
./treewidzard.sh -modelcheck PACE examples/presentation/model_checking/no_triangle.txt "$PATH_GRAPH" "$PATH_TD"
```

Expected:

```text
Result:PROPERTY SATISFIED
```

The path has no Hamiltonian cycle:

```bash
./treewidzard.sh -modelcheck PACE examples/presentation/model_checking/no_hamiltonian_cycle.txt "$PATH_GRAPH" "$PATH_TD"
```

Expected:

```text
Result:PROPERTY SATISFIED
```

The path has exactly 4 vertices:

```bash
./treewidzard.sh -modelcheck PACE examples/presentation/model_checking/vertex_count_4.txt "$PATH_GRAPH" "$PATH_TD"
```

Expected:

```text
Result:PROPERTY SATISFIED
Invariant value: INV(x)=4
```

The path does not have exactly 5 vertices:

```bash
./treewidzard.sh -modelcheck PACE examples/presentation/model_checking/vertex_count_5.txt "$PATH_GRAPH" "$PATH_TD"
```

Expected:

```text
Result:PROPERTY NOT SATISFIED
Invariant value: INV(x)=4
```

The path is a forest, so its feedback vertex set number is at most 0:

```bash
./treewidzard.sh -modelcheck PACE examples/presentation/model_checking/feedback_vertex_set_at_most_0.txt "$PATH_GRAPH" "$PATH_TD"
```

Expected:

```text
Result:PROPERTY SATISFIED
```

The diamond graph is not a forest:

```bash
./treewidzard.sh -modelcheck PACE examples/presentation/model_checking/feedback_vertex_set_at_most_0.txt "$GRAPH" "$TD"
```

Expected:

```text
Result:PROPERTY NOT SATISFIED
```

These examples use two vertices connected by parallel edges:

```bash
MULTI_GRAPH=examples/presentation/model_checking/multiedge_pair.gr
MULTI_TD=examples/presentation/model_checking/multiedge_pair.td
```

The graph has multiple edges:

```bash
./treewidzard.sh -modelcheck PACE examples/presentation/model_checking/has_multiple_edges.txt "$MULTI_GRAPH" "$MULTI_TD"
```

Expected:

```text
Result:PROPERTY SATISFIED
```

The statement that the graph has no multiple edges is false:

```bash
./treewidzard.sh -modelcheck PACE examples/presentation/model_checking/no_multiple_edges.txt "$MULTI_GRAPH" "$MULTI_TD"
```

Expected:

```text
Result:PROPERTY NOT SATISFIED
```

## Small ATP examples

Every treewidth-0 graph is 1-colorable:

```bash
./treewidzard.sh -atp tw=0 BreadthFirstSearch examples/presentation/atp/one_colorable.txt
```

Expected:

```text
Result: PROPERTY SATISFIED
```

Not every treewidth-1 graph is 1-colorable:

```bash
./treewidzard.sh -atp tw=1 BreadthFirstSearch examples/presentation/atp/one_colorable.txt
```

Expected:

```text
Result: PROPERTY NOT SATISFIED
Counterexample found
```

Every treewidth-1 graph is 2-colorable:

```bash
./treewidzard.sh -atp tw=1 BreadthFirstSearch examples/presentation/atp/two_colorable.txt
```

Expected:

```text
Result: PROPERTY SATISFIED
```

Not every treewidth-2 graph is 2-colorable:

```bash
./treewidzard.sh -atp tw=2 BreadthFirstSearch examples/presentation/atp/two_colorable.txt
```

Expected:

```text
Result: PROPERTY NOT SATISFIED
Counterexample found
```

Every treewidth-2 graph is 3-colorable:

```bash
./treewidzard.sh -atp tw=2 BreadthFirstSearch examples/presentation/atp/three_colorable.txt
```

Expected:

```text
Result: PROPERTY SATISFIED
```

## Verify paper examples

Run the bundled paper example check:

```bash
./examples/system_paper/verify.sh
```

Expected final line:

```text
All system paper examples verified.
```
