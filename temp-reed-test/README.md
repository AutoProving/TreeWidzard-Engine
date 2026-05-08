This directory vendors a small local subset of the Reed ATP experiment files from:

`../thesis/thesis-mono/attachments/experiments_bundle/experiments/atp`

The original thesis files use older core names:

- `MaxDegree(...)`
- `ChromaticNumber(...)`

This bundle translates them to the current TreeWidzard core names:

- `MaximumDegree_AtLeast(...)`
- `ChromaticNumber_AtMost(...)`

Included subset:

- `true/tw1_degree_coloring_property.txt`
- `true/tw1_degree_coloring_reverse.txt`
- `true/tw1_degree_coloring_strong.txt`
- `true/pw1_degree_coloring_property.txt`
- `true/pw1_degree_coloring_strong.txt`
- `false/tw3_degree_coloring_strong.txt`
- `false/pw3_degree_coloring_strong.txt`

These are kept intentionally small so the local integration suite can check the Reed case-study semantics without pulling in the long-running thesis widths.
