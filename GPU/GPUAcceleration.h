#pragma once

#include <memory>
#include <vector>
#include <string>
#include <optional>

// Forward declarations
namespace TreeWidzard {
    class WitnessSet;
    class Bag;
    class State;
}

namespace TreeWidzard {
namespace GPU {

/**
 * GPU Acceleration Interface for TreeWidzard
 * 
 * This provides a clean interface for GPU-accelerated computations
 * while maintaining compatibility with the existing CPU implementation.
 */

// GPU device information
struct GPUDevice {
    int device_id;
    std::string name;
    size_t memory_mb;
    int compute_capability;
    bool cuda_available;
    bool opencl_available;
};

// GPU computation result
struct GPUComputationResult {
    std::vector<WitnessSet> witness_sets;
    double computation_time_ms;
    size_t memory_used_bytes;
    bool success;
};

// Abstract GPU processor interface
class IGPUProcessor {
public:
    virtual ~IGPUProcessor() = default;
    
    // Initialize GPU resources
    virtual bool initialize() = 0;
    
    // Process batch of witness set computations
    virtual GPUComputationResult processBatch(
        const std::vector<Bag>& bags,
        const std::string& core_name,
        int tree_width
    ) = 0;
    
    // Check if GPU processing is beneficial for given workload
    virtual bool shouldUseGPU(size_t batch_size, const std::string& core_name) const = 0;
    
    // Cleanup GPU resources
    virtual void cleanup() = 0;
    
    // Get device information
    virtual GPUDevice getDeviceInfo() const = 0;
};

// CUDA-based implementation
class CUDAProcessor : public IGPUProcessor {
private:
    void* cuda_context;
    void* device_memory;
    size_t allocated_memory;
    int device_id;
    
public:
    CUDAProcessor(int device = 0);
    ~CUDAProcessor();
    
    bool initialize() override;
    GPUComputationResult processBatch(
        const std::vector<Bag>& bags,
        const std::string& core_name,
        int tree_width
    ) override;
    bool shouldUseGPU(size_t batch_size, const std::string& core_name) const override;
    void cleanup() override;
    GPUDevice getDeviceInfo() const override;
    
    // CUDA-specific methods
    bool checkCUDAAvailability() const;
    void optimizeForDevice();
};

// OpenCL-based implementation (cross-platform)
class OpenCLProcessor : public IGPUProcessor {
private:
    void* opencl_context;
    void* command_queue;
    void* program;
    std::vector<void*> kernels;
    
public:
    OpenCLProcessor();
    ~OpenCLProcessor();
    
    bool initialize() override;
    GPUComputationResult processBatch(
        const std::vector<Bag>& bags,
        const std::string& core_name,
        int tree_width
    ) override;
    bool shouldUseGPU(size_t batch_size, const std::string& core_name) const override;
    void cleanup() override;
    GPUDevice getDeviceInfo() const override;
    
    // OpenCL-specific methods
    bool loadKernels(const std::string& kernel_source);
    std::vector<GPUDevice> enumerateDevices() const;
};

// GPU Manager - handles device selection and workload distribution
class GPUManager {
private:
    std::vector<std::unique_ptr<IGPUProcessor>> processors;
    std::optional<GPUDevice> selected_device;
    bool gpu_enabled;
    
public:
    GPUManager();
    ~GPUManager();
    
    // Initialize GPU subsystem
    bool initialize();
    
    // Detect available GPU devices
    std::vector<GPUDevice> detectDevices() const;
    
    // Select best GPU for TreeWidzard workloads
    bool selectOptimalDevice();
    
    // Process computation with automatic CPU/GPU selection
    GPUComputationResult processWithAutoSelection(
        const std::vector<Bag>& bags,
        const std::string& core_name,
        int tree_width
    );
    
    // Force GPU usage (for benchmarking)
    GPUComputationResult processOnGPU(
        const std::vector<Bag>& bags,
        const std::string& core_name,
        int tree_width
    );
    
    // Configuration
    void setGPUEnabled(bool enabled) { gpu_enabled = enabled; }
    bool isGPUEnabled() const { return gpu_enabled; }
    bool isGPUAvailable() const { return selected_device.has_value(); }
    
    // Statistics
    GPUDevice getCurrentDevice() const;
    void printDeviceInfo() const;
};

// Utility functions for GPU integration
namespace Utils {
    // Check system GPU capabilities
    bool hasNVIDIAGPU();
    bool hasAMDGPU();
    bool hasIntelGPU();
    
    // Memory size calculations
    size_t estimateGPUMemoryNeeded(size_t batch_size, int tree_width);
    size_t getAvailableGPUMemory();
    
    // Performance heuristics
    bool isGPUBeneficial(const std::string& core_name, size_t problem_size);
    double estimateSpeedup(const std::string& core_name, size_t batch_size);
    
    // Data transfer optimization
    bool shouldUsePinnedMemory(size_t data_size);
    void optimizeMemoryTransfers();
}

}} // namespace TreeWidzard::GPU