# TreeWidzard System Architecture

## Overview

TreeWidzard is a framework for automated theorem proving and model checking on graph structures using tree decompositions and dynamic programming. The system combines parameterized complexity theory with automated reasoning to verify graph properties efficiently.

## Core Components

### 1. Dynamic Programming Cores (DPCores)

**Purpose**: Implement specific graph properties using dynamic programming on tree decompositions.

**Location**: `DPCores/Source/`

**Key Components**:
- `ChromaticNumber_AtMost`: Verifies k-colorability
- `IndependentSet_Max`: Computes maximum independent sets
- `VertexCount`: Basic vertex counting operations
- `MaxDegree_AtLeast`: Degree-based properties

**Architecture**:
```
DynamicCore (Abstract Base)
├── Witness management
├── State transitions
└── Property evaluation

Concrete Implementations
├── ChromaticNumber_AtMost
├── IndependentSet_Max
└── ... (other properties)
```

### 2. Search Strategies

**Purpose**: Control the exploration of the search space during conjecture verification.

**Location**: `SearchStrategies/Source/`

**Available Strategies**:
- `BreadthFirstSearch`: Standard BFS exploration
- `ParallelBreadthFirstSearch`: Multi-threaded BFS
- `IsomorphismBreadthFirstSearch`: Isomorphism-aware search
- `ParallelIsomorphismBreadthFirstSearch`: Parallel + isomorphism pruning

### 3. Tree Automaton Framework

**Purpose**: Manage tree decompositions and execute dynamic programming algorithms.

**Key Classes**:
- `ConcreteTreeDecomposition`: Represents actual tree decompositions
- `InstructiveTreeDecomposition`: Parameterized decomposition templates
- `TreeAutomaton`: Coordinates DP execution
- `RunTree`: Tracks execution states and witnesses

### 4. Conjecture System

**Purpose**: Parse, represent, and evaluate logical formulas about graph properties.

**Components**:
- `Conjecture`: Main conjecture representation
- `ConjectureNode`: AST nodes for formulas
- `PropertyAssignment`: Variable to property mappings

**Supported Logic**:
- Boolean operations (AND, OR, NOT, IMPLIES)
- Arithmetic comparisons (≤, ≥, ==, <, >)
- Quantification over graph properties

### 5. Controller Layer

**Purpose**: Orchestrate the overall system execution and handle user interactions.

**Components**:
- `SearchController`: Manages search strategy execution
- `ParseController`: Handles input parsing
- `InputController`: Processes command-line arguments

## Data Flow

```
Input Files → Parsers → Conjecture + TreeDecomposition
                ↓
         Dynamic Kernel Setup
                ↓
         Search Strategy Execution
                ↓
         DP-Core Computations
                ↓
    Witness/Counterexample Generation
                ↓
           Output Results
```

## Key Design Decisions

### Plugin Architecture
- DPCores and SearchStrategies are dynamically loaded
- Enables easy extension without recompilation
- Clean separation of concerns

### Parallel Execution
- Thread-safe data structures
- Parallel search strategies for scalability
- Efficient state sharing and synchronization

### Witness Generation
- Complete counterexample/witness extraction
- Multiple output formats (GML, PACE, custom)
- Detailed execution traces for debugging

### Memory Management
- Smart pointers for automatic memory management
- Efficient state representation
- Minimal copying in critical paths

## Extension Points

### Adding New DP-Cores
1. Inherit from `DynamicCore`
2. Implement property-specific DP transitions
3. Define witness set management
4. Export via C interface

### Adding Search Strategies
1. Inherit from `SearchStrategy`
2. Implement search loop and state management
3. Handle parallelization if needed
4. Register with the strategy handler

### Supporting New Input Formats
1. Create parser in `Translation/` directory
2. Implement format-specific reader
3. Convert to internal tree decomposition format
4. Register with input controller

## Performance Considerations

### Optimization Strategies
- `-O3` compilation for maximum performance
- Efficient hash maps (parallel_hashmap)
- Minimal dynamic allocations in hot paths
- Cache-friendly data structures

### Scalability Features
- Multi-threading support
- Cluster computing integration (SLURM)
- Memory-efficient state representation
- Incremental computation where possible

## Future Architecture Directions

### Potential Improvements
1. **Distributed Computing**: Multi-node cluster support
2. **Machine Learning Integration**: Heuristic guidance for search
3. **Interactive Mode**: Real-time conjecture exploration
4. **Web Interface**: Browser-based system access

### Modularity Enhancements
1. **Core Plugin API**: Standardized plugin interface
2. **Configuration Management**: Runtime configuration system
3. **Monitoring/Profiling**: Built-in performance analysis
4. **Testing Framework**: Comprehensive unit/integration tests
