# TreeWidzard Conjecture Format and Available DP-Cores

## Conjecture File Format

TreeWidzard uses a structured text format for defining conjectures about graph properties. The format consists of two main sections:

### 1. Variable Assignment Section
Variables are assigned to DP-cores with optional parameters:

```
variable_name := CoreName(parameters)
```

### 2. Formula Section
The logical formula using the defined variables:

```
Formula
<logical_expression>
```

## Complete Conjecture Syntax

### Basic Structure
```
// Optional comments (lines starting with //)
variable1 := CoreName1(param1, param2, ...)
variable2 := CoreName2(param1, param2, ...)
...
Formula
<logical_expression_using_variables>
```

### Example: Simple 4-Colorability Check
```
x := ChromaticNumber(4)
Formula
x
```

### Example: Complex Logical Property
```
x := MaxDegree(4)
y := ChromaticNumber(4)
Formula
NOT x IMPLIES y
```
*Translation: "If the maximum degree is NOT ≥ 4, then the graph is 4-colorable"*

### Example: Quantitative Property
```
x := VertexCount()
Formula
INV(x) < 10
```
*Translation: "The vertex count is less than 10"*

## Available DP-Cores

Based on the source code analysis, TreeWidzard provides the following DP-cores:

### 1. **ChromaticNumber** (Bool Type)
- **Purpose**: Tests k-colorability of graphs
- **Parameters**: Integer k (number of colors)
- **Usage**: `x := ChromaticNumber(k)`
- **Returns**: True if graph is k-colorable, False otherwise
- **Examples**:
  ```
  x := ChromaticNumber(3)  // 3-colorable?
  x := ChromaticNumber(4)  // 4-colorable?
  ```

### 2. **IndependentSet** (Max Type)
- **Purpose**: Computes maximum independent set
- **Parameters**: None
- **Usage**: `x := IndependentSet()`
- **Returns**: Size of maximum independent set
- **Example**:
  ```
  x := IndependentSet()
  Formula
  INV(x) >= 5  // Independent set of size ≥ 5
  ```

### 3. **VertexCount** (Min Type)
- **Purpose**: Counts vertices in the graph
- **Parameters**: None
- **Usage**: `x := VertexCount()`
- **Returns**: Number of vertices
- **Example**:
  ```
  x := VertexCount()
  Formula
  INV(x) == 10  // Exactly 10 vertices
  ```

### 4. **MaxDegree** (AtLeast Type)
- **Purpose**: Tests if maximum degree is at least k
- **Parameters**: Integer k (degree threshold)
- **Usage**: `x := MaxDegree(k)`
- **Returns**: True if max degree ≥ k, False otherwise
- **Example**:
  ```
  x := MaxDegree(3)
  Formula
  NOT x  // Maximum degree < 3
  ```

### 5. **CliqueNumber** (AtLeast Type)
- **Purpose**: Tests if clique number is at least k
- **Parameters**: Integer k (clique size threshold)
- **Usage**: `x := CliqueNumber(k)`
- **Returns**: True if clique number ≥ k, False otherwise
- **Example**:
  ```
  x := CliqueNumber(4)
  Formula
  x  // Has a 4-clique
  ```

### 6. **CliqueNumberSimpleGraphs** (AtLeast Type)
- **Purpose**: Optimized clique detection for simple graphs
- **Parameters**: Integer k (clique size threshold)
- **Usage**: `x := CliqueNumberSimpleGraphs(k)`
- **Returns**: True if clique number ≥ k, False otherwise

### 7. **HasMultipleEdges** (Bool Type)
- **Purpose**: Tests if graph has multiple edges
- **Parameters**: None
- **Usage**: `x := HasMultipleEdges()`
- **Returns**: True if graph has multiple edges, False otherwise

## Core Types and Return Values

### Core Types
- **Bool**: Returns true/false for property satisfaction
- **Max**: Returns maximum value of a property
- **Min**: Returns minimum value of a property  
- **AtLeast**: Returns true if property value ≥ threshold

### Using Core Results in Formulas

#### Direct Boolean Use
```
x := ChromaticNumber(3)
Formula
x  // True if 3-colorable
```

#### Negation
```
x := MaxDegree(5)
Formula
NOT x  // True if max degree < 5
```

#### With INV() for Quantitative Properties
```
x := IndependentSet()
Formula
INV(x) >= 3  // Independent set size ≥ 3
```

## Logical Operators in Formulas

### Boolean Operators
- **AND**: `x AND y`
- **OR**: `x OR y`
- **NOT**: `NOT x`
- **IMPLIES**: `x IMPLIES y` (logical implication)
- **IFF**: `x IFF y` (if and only if)

### Comparison Operators (for INV expressions)
- **<**: Less than
- **<=**: Less than or equal
- **>**: Greater than
- **>=**: Greater than or equal
- **==**: Equal to

### Arithmetic Operators (for INV expressions)
- **+**: Addition
- **-**: Subtraction
- **\***: Multiplication
- **/**: Division

## Advanced Examples

### 1. Brooks' Theorem Check
```
deg := MaxDegree(k)
col := ChromaticNumber(k)
Formula
deg IMPLIES col
```

### 2. Independence vs Vertex Cover
```
alpha := IndependentSet()
n := VertexCount()
Formula
INV(alpha) + INV(n) - INV(alpha) == INV(n)
```

### 3. Degree-Chromatic Number Relationship
```
max_deg := MaxDegree(3)
chromatic := ChromaticNumber(4)
Formula
NOT max_deg IMPLIES chromatic
```

### 4. Complex Quantitative Property
```
vertices := VertexCount()
independent := IndependentSet()
Formula
INV(independent) * 2 >= INV(vertices)
```

## Error Handling and Validation

### Common Syntax Errors
1. **Missing Formula keyword**: Always include `Formula` before the logical expression
2. **Undefined variables**: All variables in the formula must be defined in the assignment section
3. **Invalid core names**: Core names are case-sensitive and must match exactly
4. **Wrong parameter types**: Check parameter requirements for each core
5. **Malformed expressions**: Ensure proper parentheses and operator precedence

### Valid Core Names (Case-Sensitive)
- `ChromaticNumber`
- `IndependentSet`
- `VertexCount`
- `MaxDegree`
- `CliqueNumber`
- `CliqueNumberSimpleGraphs`
- `HasMultipleEdges`

## Usage with TreeWidzard

### Command Line Examples
```bash
# Model checking with PACE format
./treewidzard -modelcheck PACE conjecture.txt graph.gr decomposition.td

# Automated theorem proving
./treewidzard -atp tw=4 -pl -nthreads 8 ParallelBreadthFirstSearch conjecture.txt

# With path width
./treewidzard -atp pw=3 -premise -pl BreadthFirstSearch conjecture.txt
```

### Available Search Strategies
- `BreadthFirstSearch`: Standard breadth-first search
- `ParallelBreadthFirstSearch`: Multi-threaded BFS
- `IsomorphismBreadthFirstSearch`: BFS with isomorphism pruning
- `ParallelIsomorphismBreadthFirstSearch`: Parallel BFS with isomorphism pruning

## Tips for Writing Effective Conjectures

### 1. Start Simple
Begin with single-variable conjectures to test basic properties:
```
x := ChromaticNumber(3)
Formula
x
```

### 2. Use Meaningful Variable Names
```
three_colorable := ChromaticNumber(3)
four_colorable := ChromaticNumber(4)
Formula
three_colorable IMPLIES four_colorable
```

### 3. Combine Properties Logically
```
high_degree := MaxDegree(5)
many_colors := ChromaticNumber(6)
Formula
high_degree IMPLIES many_colors
```

### 4. Test Quantitative Bounds
```
vertices := VertexCount()
independence := IndependentSet()
Formula
INV(independence) <= INV(vertices) / 2
```

### 5. Use Comments for Clarity
```
// Testing Vizing's theorem: chromatic index ≤ max degree + 1
max_deg := MaxDegree(3)
edge_chromatic := ChromaticNumber(4)  // Edge coloring approximation
Formula
max_deg IMPLIES edge_chromatic
```

This comprehensive format allows TreeWidzard to express a wide range of graph-theoretic properties and relationships, making it suitable for both automated theorem proving and specific graph property verification tasks.