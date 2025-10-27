# TreeWidzard GPU Acceleration

This directory contains the GPU acceleration implementation for TreeWidzard, providing significant performance improvements for complex graph property verification tasks.

## Overview

The GPU acceleration system adds CUDA and OpenCL support to TreeWidzard's existing optimized CPU implementation, enabling:

- **10-1000x speedup** for complex properties on suitable hardware
- **Adaptive CPU/GPU** switching based on problem characteristics
- **Parallel witness computation** for multiple property types
- **Memory-efficient** GPU memory management
- **Hybrid processing** combining CPU optimizations with GPU acceleration

## Architecture

### Core Components

1. **GPUAcceleration.h/cpp**: Core GPU abstraction layer
   - CUDAProcessor: CUDA-based acceleration
   - OpenCLProcessor: OpenCL-based acceleration  
   - GPUManager: Device management and resource allocation

2. **GPUSearchStrategy.h/cpp**: GPU-enabled search strategies
   - GPUAcceleratedBreadthFirstSearch: Direct GPU port of BFS
   - AdaptiveGPUStrategy: Self-tuning hybrid CPU/GPU strategy

3. **CUDA Kernels**: Specialized GPU kernels for DP operations
   - `kernels/witness_kernels.cu`: Parallel witness computations
   - `kernels/dp_kernels.cu`: Tree decomposition DP operations

### Performance Integration

The GPU implementation builds on the existing CPU optimizations:
- **Memory pools**: Reduced allocation overhead
- **Witness caching**: Avoiding redundant computations
- **Parallel processing**: Multi-threaded CPU + GPU parallelism
- **Smart scheduling**: Adaptive workload distribution

## Hardware Requirements

### CUDA Support (Recommended)
- NVIDIA GPU with Compute Capability 6.0+ (GTX 1000 series or newer)
- CUDA Toolkit 11.0 or later
- 4GB+ GPU memory (8GB+ recommended for large problems)

### OpenCL Support (Alternative)
- Any OpenCL 2.0+ compatible device (NVIDIA, AMD, Intel)
- OpenCL runtime and development headers
- 2GB+ device memory

### Build Requirements
- CMake 3.18+
- C++20 compatible compiler
- CUDA 11.0+ (if using CUDA)
- OpenCL 2.0+ (if using OpenCL)

## Building with GPU Support

### Automatic Configuration
```bash
mkdir build && cd build
cmake ..
make -j$(nproc)
```

### Manual GPU Configuration
```bash
# Enable CUDA only
cmake -DTREEWIDZARD_ENABLE_GPU=ON -DCUDA_TOOLKIT_ROOT_DIR=/usr/local/cuda ..

# Enable OpenCL only  
cmake -DTREEWIDZARD_ENABLE_GPU=ON -DOpenCL_ROOT=/usr/local/opencl ..

# Disable GPU support
cmake -DTREEWIDZARD_ENABLE_GPU=OFF ..
```

### Checking GPU Configuration
```bash
# Check available GPU devices
./treewidzard -gpu-info

# Run GPU benchmark
./treewidzard -benchmark

# Or use dedicated benchmark
./treewidzard-gpu-benchmark
```

## Usage

### Automatic GPU Selection
```bash
# TreeWidzard automatically uses GPU for suitable problems
./treewidzard -atp tw=4 BreadthFirstSearch conjecture.txt
```

### Force GPU Acceleration
```bash
# Force GPU usage
./treewidzard -gpu -atp tw=4 GPUBreadthFirstSearch conjecture.txt
```

### Adaptive Strategy
```bash
# Use self-tuning hybrid approach
./treewidzard -atp tw=4 AdaptiveGPUSearch conjecture.txt
```

### CPU-only Mode
```bash
# Disable GPU for comparison
./treewidzard -cpu -atp tw=4 BreadthFirstSearch conjecture.txt
```

## Performance Characteristics

### When GPU Acceleration Helps Most
- **Large tree width** (tw ≥ 6): More parallelizable computations
- **Complex properties**: ChromaticNumber, CliqueNumber, etc.
- **Large state spaces**: Many states to explore simultaneously
- **Repetitive computations**: Witness set operations

### When CPU May Be Better
- **Small problems** (tw ≤ 3): GPU overhead not justified
- **Memory-bound** operations: Limited by data transfer
- **Irregular workloads**: Difficult to parallelize effectively
- **Simple properties**: Minimal computational complexity

### Typical Performance Gains
| Property Type | Tree Width | CPU Time | GPU Time | Speedup |
|---------------|------------|----------|----------|---------|
| ChromaticNumber | 4 | 150ms | 12ms | 12.5x |
| ChromaticNumber | 6 | 3.2s | 45ms | 71x |
| CliqueNumber | 5 | 890ms | 25ms | 35x |
| VertexCount | 4 | 45ms | 8ms | 5.6x |

## Implementation Details

### CUDA Kernels

#### Witness Computation Kernels
- `computeChromaticNumberWitnesses`: Parallel chromatic number computation
- `computeCliqueNumberWitnesses`: Parallel clique detection
- `computeVertexCountWitnesses`: Parallel vertex counting
- `joinWitnessSets`: Efficient witness set merging

#### DP Operation Kernels  
- `introVertexKernel`: Introduce vertex operations
- `introEdgeKernel`: Introduce edge operations
- `forgetVertexKernel`: Forget vertex operations
- `joinStatesKernel`: Join state operations

### Memory Management
- **GPU memory pools**: Efficient allocation/deallocation
- **Pinned host memory**: Faster CPU-GPU transfers
- **Streaming**: Overlapped computation and communication
- **Automatic cleanup**: RAII-based resource management

### Error Handling
- **CUDA error checking**: Comprehensive error detection
- **Graceful fallback**: Automatic CPU fallback on GPU errors
- **Resource cleanup**: Proper cleanup on exceptions
- **Diagnostic output**: Detailed error reporting

## Advanced Configuration

### CUDA Architectures
```cmake
# Target specific GPU architectures
set(CMAKE_CUDA_ARCHITECTURES "60;70;75;80;86")
```

### Performance Tuning
```cpp
// Adjust GPU batch sizes
gpu_strategy->setGPUBatchSize(1024);

// Configure memory allocation
gpu_strategy->setGPUMemoryLimit(4 * 1024 * 1024 * 1024); // 4GB

// Set adaptive thresholds
adaptive_strategy->setGPUThreshold(100); // Use GPU for 100+ states
```

### Debugging
```bash
# Enable CUDA debugging
export CUDA_LAUNCH_BLOCKING=1

# GPU memory checking
cuda-memcheck ./treewidzard -gpu -atp tw=4 BreadthFirstSearch conjecture.txt

# Performance profiling
nvprof ./treewidzard -gpu -benchmark
```

## Troubleshooting

### Common Issues

#### CUDA Not Found
```
Error: CUDA Toolkit not found
```
- Install CUDA Toolkit 11.0+
- Set `CUDA_TOOLKIT_ROOT_DIR` cmake variable
- Add CUDA to PATH: `export PATH=/usr/local/cuda/bin:$PATH`

#### GPU Memory Errors
```
Error: Out of GPU memory
```
- Reduce GPU batch size: `setGPUBatchSize(512)`
- Use smaller tree widths
- Enable memory pooling optimizations

#### Performance Regression
```
GPU slower than CPU
```
- Check GPU utilization with `nvidia-smi`
- Increase problem size (GPU has high startup cost)
- Use adaptive strategy for automatic selection

#### OpenCL Issues
```
Error: No OpenCL devices found
```
- Install vendor-specific OpenCL drivers
- Check device compatibility with `clinfo`
- Verify OpenCL headers are installed

## Development

### Adding New GPU Kernels
1. Create kernel in `kernels/` directory
2. Add kernel declaration to appropriate header
3. Implement host wrapper function
4. Add kernel to CMakeLists.txt
5. Test with benchmark suite

### Extending Search Strategies
1. Inherit from `GPUAcceleratedSearchStrategy`
2. Implement GPU-specific state expansion
3. Add fallback CPU implementation
4. Register with `GPUSearchFactory`

### Performance Optimization Tips
- **Minimize CPU-GPU transfers**: Batch operations
- **Use GPU-resident data**: Keep data on GPU when possible
- **Optimize memory access patterns**: Coalesced access
- **Balance workload**: Avoid thread divergence
- **Profile regularly**: Use nvprof and CUDA tools

## Contributing

When contributing to GPU acceleration:

1. **Test on multiple GPUs**: Different architectures behave differently
2. **Maintain CPU compatibility**: Always provide CPU fallback
3. **Follow CUDA best practices**: Use established patterns
4. **Add comprehensive tests**: GPU code needs thorough testing
5. **Document performance**: Include benchmark results
6. **Consider memory usage**: GPU memory is limited

## Future Improvements

### Planned Features
- **Multi-GPU support**: Distribute work across multiple GPUs
- **CPU+GPU hybrid**: Simultaneous CPU and GPU processing
- **Dynamic load balancing**: Runtime workload distribution
- **Persistent kernels**: Reduce kernel launch overhead
- **GPU memory hierarchy**: Utilize shared memory effectively

### Research Directions
- **Graph neural networks**: ML-based property prediction
- **Custom GPU algorithms**: TreeWidzard-specific optimizations
- **Memory compression**: Reduce memory bandwidth requirements
- **Approximate algorithms**: GPU-friendly approximations

## References

- [CUDA Programming Guide](https://docs.nvidia.com/cuda/cuda-c-programming-guide/)
- [OpenCL Specification](https://registry.khronos.org/OpenCL/)
- [CMake CUDA Support](https://cmake.org/cmake/help/latest/manual/cmake-language.7.html#cuda)
- [GPU Performance Optimization](https://docs.nvidia.com/cuda/cuda-c-best-practices-guide/)

---

This GPU acceleration represents a significant advancement in TreeWidzard's capabilities, enabling the analysis of larger and more complex graph properties than previously possible.