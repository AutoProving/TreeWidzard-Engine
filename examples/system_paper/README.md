# System Paper Examples

This folder collects the concrete usage examples from the TreeWidzard system paper, adapted to the current core names used in this repository.

Structure:

- `atp/`: automated theorem proving examples
- `model_checking/`: model-checking examples using PACE and ITD inputs
- `invariants/`: invariant-computation examples
- `property_files/`: small standalone property-file examples
- `verify.sh`: runs the paper examples and checks the expected outcomes

Example commands:

```bash
./treewidzard.sh -atp tw=4 -pl ParallelBreadthFirstSearch examples/system_paper/atp/five_colorable.txt
./treewidzard.sh -modelcheck PACE examples/system_paper/model_checking/three_colorable.txt examples/system_paper/model_checking/diamond.gr examples/system_paper/model_checking/diamond.td
./treewidzard.sh -modelcheck ITD examples/system_paper/model_checking/three_colorable.txt examples/system_paper/model_checking/diamond.itd
./treewidzard.sh -modelcheck PACE examples/system_paper/invariants/independence_number.txt examples/system_paper/model_checking/diamond.gr examples/system_paper/model_checking/diamond.td
```
