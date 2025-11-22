# ATP Conjecture Set (width-bounded)

Purpose: width-bounded "true vs false" conjectures for treewidth/pathwidth up to 8. No input graphs required; use `treewidzard -atp tw=<k> ... <file>` (or `pw=<k>`). Tightness: the same file often becomes false when you bump the width by 1 (e.g., K_{k+2} refutes `ChromaticNumber(k+1)` at width k+1).

True theorems for given width (treewidth or pathwidth):
- `tw1_bipartite.txt` -> all width-1 graphs are 2-colourable.
- `tw1_feedback_vertex_set.txt` -> width-1 graphs are acyclic.
- `tw1_independent_half.txt` -> forests have alpha >= n/2.
- `vc2_implies_3colorable.txt` -> tau <= 2 implies chi <= 3 (general pattern: VC(k) implies chi <= k+1).
- `fvs1_implies_3colorable.txt` -> FVS <= 1 implies chi <= 3 (pattern: FVS(k) implies chi <= k+2).
- `3colorable_forbids_K4.txt` -> chi <= 3 forbids K4 (pattern: chi <= k implies omega <= k).
- `two_colorable_large_independent.txt` -> chi <= 2 implies 2*alpha >= n; `three_colorable_large_independent.txt` -> chi <= 3 implies 3*alpha >= n.
- `chromatic_monotone_3_to_4.txt` -> chi <= 3 implies chi <= 4 (pattern: chi <= k implies chi <= k+1).
- Colouring ladder (chi <= width+1): `tw1_bipartite.txt` (chi<=2), `tw2_three_colorable.txt` (chi<=3), `four_colorable.txt` (chi<=4), `five_colorable.txt` (chi<=5), plus `chi_le_6/7/8/9.txt` for widths 5–8.
- Clique upper bounds: `no_K3/4/5/6/7/8/9/10_clique.txt` encode omega <= width+1 (run `no_K{s}_clique.txt` with width s-2).
- `perfect_matching.txt` stays a normal existence property (not width-specific).

Deliberately false / tightness checks:
- Run any `ChromaticNumber(k+1)` file with width k+1 (e.g., `four_colorable.txt` with `tw=4`) to get a K_{k+2} counterexample.
- Run `no_K{s}_clique.txt` with width >= s-1 (e.g., `no_K4_clique.txt` with `tw=3`) to admit K_s.
- `all_bipartite.txt` is false for width >=2 (odd cycles).
- `all_have_perfect_matching.txt` is false for essentially any width >=1.
- `vc2_implies_2colorable_wrong.txt`, `fvs1_implies_2colorable_wrong.txt`, `K4_implies_3colorable_wrong.txt` are explicit wrong variants.

Examples:
```
# True at width 1, false at width 2 (C5)
./treewidzard -atp tw=1 -pl ParallelBreadthFirstSearch system-paper/ATP/tw1_independent_half.txt
./treewidzard -atp tw=2 -pl ParallelBreadthFirstSearch system-paper/ATP/tw1_independent_half.txt

# Width-colouring ladder
./treewidzard -atp tw=5 -pl ParallelBreadthFirstSearch system-paper/ATP/chi_le_6.txt   # true
./treewidzard -atp tw=6 -pl ParallelBreadthFirstSearch system-paper/ATP/chi_le_6.txt   # false (K7)

# Clique bound
./treewidzard -atp tw=2 -pl ParallelBreadthFirstSearch system-paper/ATP/no_K4_clique.txt   # true
./treewidzard -atp tw=3 -pl ParallelBreadthFirstSearch system-paper/ATP/no_K4_clique.txt   # false (K4)
```
