# TreeWidzard Optimization Review

## Priority Legend

- **P0 — Critical**: Dominates runtime or memory in typical workloads. Fix these first.
- **P1 — High**: Measurable impact on moderate-to-large inputs.
- **P2 — Medium**: Noticeable in profiling; worth fixing in a focused pass.
- **P3 — Low**: Minor or affects cold paths only.

---

## P0 — Critical Issues

### 1. `set<set<unsigned>>` in ChromaticNumber witness — exponential copy cost

**File:** `DPCores/Source/ChromaticNumber_AtMost.h`, line 13

The witness stores `set<set<unsigned>> partialColoring`. Every `clone()` deep-copies
this nested structure — O(k × width) per clone where k is the color count.
`intro_v_implementation` clones once per existing color class plus one for a new class,
so a single intro_v on a k-colored witness triggers k+1 deep copies of a nested set.

**Why it matters:** ChromaticNumber is the most commonly used core. At width 5 with k=4,
each intro_v creates ~5 clones × ~20 element set copies = ~100 set allocations. This
dominates runtime for chromatic number computations.

**Fix:** Replace `set<set<unsigned>>` with `vector<set<unsigned>>` (sorted on access for
comparison). Cloning a vector of sets is cheaper (no tree rebalancing). For further gains,
represent color classes as bitmasks when width ≤ 32: `vector<uint32_t> colorClasses`
makes clone O(k) and comparison O(k) instead of O(k × width).

---

### 2. WitnessSetTypeOne global mutex — serialization bottleneck

**File:** `Kernel/WitnessSet.h`, lines 129–132, 304–317

All WitnessSetTypeOne instances (across all template instantiations of the same type T)
share a single static `witnessMutex`. Every `insert()` acquires this lock to check/add to
`allWitnesses` (a global `std::map`). This serializes all witness insertions across any
concurrent or interleaved operations.

Even in single-threaded execution, the lock acquisition/release overhead is paid on
every single witness insertion. In multi-threaded BFS exploration, this becomes a hard
serialization point.

**Why it matters:** Witness insertion is the innermost operation in CoreWrapper — called
for every witness × every tree decomposition operation. This is the hottest code path.

**Fix (single-threaded):** Use a thread-local witness registry, or remove the mutex
entirely behind a compile-time flag for single-threaded builds.
**Fix (multi-threaded):** Replace the global `std::map` + mutex with a concurrent hash
map (e.g., `tbb::concurrent_hash_map` or a lock-free structure). Consider sharding
by witness hash to reduce contention.

---

### 3. WitnessSetTypeTwo `insert()` — O(n) shift per insertion

**File:** `Kernel/WitnessSet.h`, lines 448–454

```cpp
container.insert(it, std::move(ws));  // O(n) element shift
```

`WitnessSetTypeTwo` stores witnesses in a sorted `vector<shared_ptr<Witness>>`.
`insert()` does `lower_bound` (O(log n)) then `vector::insert` at that position (O(n)
due to shifting). When CoreWrapper collects results by calling `insert()` in a loop,
the total cost becomes O(n²) over all insertions.

**Why it matters:** CoreWrapper calls insert once per output witness. For cores that
produce many witnesses (e.g., ChromaticNumber with high k, IndependentSet branching),
the quadratic insertion cost dominates.

**Fix:** Accumulate witnesses into an unsorted vector during the operation, then sort
and deduplicate once at the end (in `clean_implementation` or a post-pass). This
converts O(n²) insertion to O(n log n) sort. Alternatively, use `std::set` or a
B-tree for O(log n) insertion if random-access iteration isn't needed.

---

### 4. BFS Join — O(n²) pair enumeration per bag signature

**File:** `Kernel/BreadthFirstTraversal.cpp`, lines 193–210

```cpp
for (const auto &existing_state : bucket_it->second) {
    State::ptr new_state = kernel.join(state, existing_state);
```

For every frontier state, the BFS iterates over ALL previously seen states with the
same bag signature and attempts a join with each. If k states share a bag signature,
this produces O(k²) join operations per iteration. Each join operation is expensive
(iterates all witness pairs between two witness sets).

**Why it matters:** At width ≥ 3 with tree_width mode, bag signature buckets grow
rapidly. The join explosion often dominates total BFS runtime.

**Fix:** Track which (state, state) pairs have already been joined. Use witness-based
signatures to split buckets further — two states with identical bags but incompatible
witness patterns will produce empty joins. Pre-filter with a cheap compatibility check
before invoking the full join.

---

### 5. FeedbackVertexSet union-find — O(n) per find without path compression

**File:** `DPCores/Source/FeedbackVertexSet_AtMost.h`, lines 150–152 (intro_e),
lines 85–93 (normalize)

The union-find is stored as `map<unsigned, unsigned> comp` with no path compression.
`find_root` does a linear walk up the parent chain. `normalize()` iterates all vertices
twice to canonicalize representatives. Both are called on every intro_v, intro_e,
forget_v, and join operation.

**Why it matters:** Without path compression, find degrades to O(width) per call.
Combined with normalize being called on every operation, the cost per tree decomposition
step is O(width²) instead of the near-O(width) achievable with proper union-find.

**Fix:** Implement path compression in `find_root`. Replace the `normalize()` approach
with union-by-rank. Store the union-find as a flat `vector<unsigned>` indexed by vertex
ID (requires mapping vertices to indices 0..width-1).

---

## P1 — High Impact Issues

### 6. DynamicKernel creates redundant Bag copies on every operation

**File:** `Kernel/DynamicKernel.cpp`, lines 27, 32 (intro_v); 47, 52 (intro_e); etc.

```cpp
Bag next_bag = bag;          // copy #1
aux->set_bag(next_bag.intro_v(i));  // intro_v returns a NEW bag (copy #2)
```

Every DynamicKernel operation (intro_v, intro_e, forget_v) copies the Bag twice:
once into `next_bag`, then `intro_v()` creates another copy internally. The original
`bag` reference isn't mutated, so the first copy is unnecessary.

**Fix:** Call `bag.intro_v(i)` directly:
```cpp
aux->set_bag(bag.intro_v(i));  // Only one copy
```

---

### 7. BagSignature copies elements on every creation

**File:** `Kernel/BreadthFirstTraversal.cpp`, lines 31–34, 194, 225

```cpp
signature.elements.assign(elements.begin(), elements.end());  // copies set to vector
```

`make_bag_signature` is called for every state during join bucket lookup (line 194)
and state insertion (line 225). Each call copies the bag's `set<unsigned>` into a
`vector<unsigned>`.

**Fix:** Cache the bag signature hash directly inside the State or Bag object. Compute
it once during State construction and store it alongside the bag. This eliminates
repeated set-to-vector copies and hash recomputation.

---

### 8. HasMultipleEdges join copies edge sets 4 times

**File:** `DPCores/Source/HasMultipleEdges.h`, lines 286–307

```cpp
set<pair<unsigned, unsigned>> wOneSet = w1.edgeSet;  // copy 1
set<pair<unsigned, unsigned>> wTwoSet = w2.edgeSet;  // copy 2
set_intersection(...);
// If no intersection:
set<pair<unsigned, unsigned>> wOneSet = w1.edgeSet;  // copy 3!
set<pair<unsigned, unsigned>> wTwoSet = w2.edgeSet;  // copy 4!
set_union(...);
```

Four unnecessary copies of potentially large edge sets. The copies inside the else
branch shadow the outer variables but re-copy the same data.

**Fix:** Single-pass merge:
```cpp
auto it1 = w1.edgeSet.begin(), it2 = w2.edgeSet.begin();
bool has_dup = false;
while (it1 != w1.edgeSet.end() && it2 != w2.edgeSet.end()) {
    if (*it1 == *it2) { has_dup = true; break; }
    if (*it1 < *it2) ++it1; else ++it2;
}
```
No copies needed — just iterate both sorted sets in parallel.

---

### 9. MinorContainment union-find — same issue as FeedbackVertexSet

**File:** `DPCores/Source/MinorContainment_Generic.h`, lines 24–33

`find_root` has no path compression and walks the parent chain linearly.
Component analysis in intro_e (lines 152–165) does O(width²) pairwise checks.

**Fix:** Same as issue #5 — proper union-find with path compression and union-by-rank.

---

### 10. IndependentSet witness explosion — 2^width potential witnesses

**File:** `DPCores/Source/IndependentSet_Max.h`, lines 75–85

`intro_v_implementation` always produces 2 witnesses (add vertex or skip), leading
to exponential witness count without aggressive pruning.

The `clean_implementation` runs after each step, but its sort is O(n log n × width)
where n can be exponential.

**Fix:** Implement dominance pruning during intro_v: if a witness with strictly larger
`size` and subset `used` set exists, the dominated witness can be dropped immediately.
This keeps the witness set polynomial. Alternatively, prune in clean using an
incremental approach rather than full sort.

---

### 11. State comparison — redundant bag comparison in operator<

**File:** `Kernel/State.cpp`, lines 148–166

```cpp
if (lhs_bag < rhs_bag) return true;
// ...
else if (lhs_bag == rhs_bag) {  // compares bag AGAIN
```

The bag is compared twice. Since `lhs_bag < rhs_bag` returned false, we should check
`rhs_bag < lhs_bag` to determine equality, OR use a three-way comparison.

**Fix:**
```cpp
if (lhs_bag < rhs_bag) return true;
if (rhs_bag < lhs_bag) return false;
// Bags are equal; compare witness sets
```

---

### 12. HamiltonianCycle/Path join — linear search in adjacency lists

**File:** `DPCores/Source/HamiltonianCycle.h`, lines 192–198

Edge removal in the colored adjacency graph does linear search:
```cpp
for (auto it = U.begin(); it != U.end(); ++it) {
    if (it->v == v && (color == 0 || it->c == color)) { U.erase(it); break; }
}
```

**Fix:** Use `unordered_map<U64, unordered_set<U64>>` for O(1) lookup, or at minimum
`unordered_map<U64, vector<Edge>>` with swap-and-pop removal.

---

## P2 — Medium Impact Issues

### 13. Virtual dispatch through BaseIterator in hot loops

**File:** `Kernel/WitnessSet.h`, lines 50–63

Every `operator++()` and `operator*()` on a WitnessSet iterator goes through virtual
dispatch. CoreWrapper iterates witnesses in every operation:
```cpp
for (const auto &w_ptr : witnessSet)  // virtual call per iteration
```

**Fix:** Provide typed iteration methods on WitnessSetTypeOne/Two that bypass the
virtual BaseIterator. CoreWrapper knows the concrete type at compile time and can
call them directly.

---

### 14. Hasher created per hash computation

**File:** `Kernel/WitnessSet.h`, line 221

```cpp
auto h = Hasher(0);  // new Hasher every time
ptr->hash(h);
```

**Fix:** Use a statically allocated or thread-local Hasher that's reset between uses.

---

### 15. CoreWrapper creates shared_ptr<WitnessSet> per operation

**File:** `Kernel/CoreWrapper.h` — intro_v, intro_e, forget_v all do:
```cpp
auto res = std::make_shared<WitnessSet>();
```

Each DynamicCore operation allocates a new WitnessSet on the heap via shared_ptr.
With multiple cores per kernel, each tree decomposition step produces `cores.size()`
heap allocations.

**Fix:** Use a WitnessSet pool or stack-allocate where lifetime is bounded. Consider
`unique_ptr` instead of `shared_ptr` where ownership is unambiguous (CoreWrapper
creates it, passes to State, never shares).

---

### 16. PerfectMatching — linear reverse pair lookup

**File:** `DPCores/Source/PerfectMatching.h`, lines 86–98

`is_paired` searches the map forward for `v`, then does a linear scan of all pairs
to check if `v` appears as a value.

**Fix:** Maintain a bidirectional map: `pairs` for u→v and `reverse_pairs` for v→u.

---

### 17. VertexCover clean uses `map<set<unsigned>, unsigned>` — set as key

**File:** `DPCores/Source/VertexCover_AtMost.h`, line 118

The clean operation deduplicates by using the selected vertex set `S` as a map key.
`std::map` with `set<unsigned>` keys means O(width) comparison per lookup.

**Fix:** Hash the set and use `unordered_map` with a set hasher.

---

## P3 — Low Impact / Design Issues

### 18. WitnessSetTypeOne mask grows one byte at a time

**File:** `Kernel/WitnessSet.h`, lines 319–320

```cpp
while (idx >= static_cast<int>(8 * mask.size()))
    mask.push_back(0);
```

**Fix:** Use exponential growth: `mask.resize(std::max(mask.size() * 2, (idx/8) + 1), 0)`

---

### 19. CorePrelude.h includes everything — compilation overhead

**File:** `CorePrelude.h`

Every DP core includes CorePrelude.h which pulls in Bag.h, CoreWrapper.h, WitnessSet.h,
WitnessWrapper.h, DynamicCore.h, plus `<set>`, `<map>`, `<vector>`, `<memory>`,
`<algorithm>`, `<iostream>`, etc. Any change to any kernel header recompiles all cores.

**Fix:** Use forward declarations where possible. Split CorePrelude into smaller focused
headers. Use precompiled headers for the STL includes.

---

### 20. `DynamicKernel` stores raw pointers to cores

**File:** `Kernel/DynamicKernel.h`, line 15

```cpp
std::vector<DynamicCore *> cores;
```

No ownership semantics. The `InputController` fix (adding `ownedCores`) partially
addresses this, but the kernel itself can't reason about core lifetime.

**Fix:** Use `std::vector<DynamicCore *>` (non-owning) with a clear documentation
contract, or switch to `shared_ptr<DynamicCore>` if shared ownership is needed.

---

### 21. `getCoreByVar` / `getIndexByVar` take `std::string` by value

**File:** `Kernel/DynamicKernel.cpp`, lines 137, 146

```cpp
DynamicCore* DynamicKernel::getCoreByVar(std::string var)
```

Copies the string on every call. Should take `const std::string &`.

---

## Summary by Impact Area

| Area | P0 | P1 | P2 | P3 | Total |
|------|----|----|----|----|-------|
| Witness data structures | 2 | 2 | 2 | 1 | 7 |
| BFS traversal | 1 | 1 | 0 | 0 | 2 |
| DP Core algorithms | 1 | 3 | 1 | 0 | 5 |
| State/Bag operations | 0 | 2 | 1 | 0 | 3 |
| Architecture/Design | 1 | 0 | 1 | 3 | 5 |

## Recommended Fix Order

1. **ChromaticNumber data structure** (#1) — largest single impact on common workloads
2. **WitnessSetTypeTwo insertion** (#3) — affects all cores using TypeTwo
3. **BFS join quadratic** (#4) — dominates tree_width mode runtime
4. **FeedbackVertexSet + MinorContainment union-find** (#5, #9) — easy fix, large gain
5. **DynamicKernel redundant bag copies** (#6) — one-line fix, immediate gain
6. **HasMultipleEdges join copies** (#8) — straightforward fix
7. **WitnessSetTypeOne mutex** (#2) — important for scaling, more complex to fix
8. **State comparison** (#11) — easy fix
9. **BagSignature caching** (#7) — moderate effort, good payoff
10. Everything else in priority order
