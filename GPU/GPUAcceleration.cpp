#include "GPUAcceleration.h"
#include "../Kernel/WitnessSet.h"
#include "../Kernel/Bag.h"
#include "../Performance/PerformanceTimer.h"
#include <iostream>
#include <algorithm>
#include <cmath>

// Check for CUDA availability at compile time
#ifdef __CUDACC__
#include <cuda_runtime.h>
#include <device_launch_parameters.h>
#define CUDA_AVAILABLE 1
#else
#define CUDA_AVAILABLE 0
#endif

// Check for OpenCL availability
#ifdef CL_VERSION_2_0
#include <CL/cl.hpp>
#define OPENCL_AVAILABLE 1
#else
#define OPENCL_AVAILABLE 0
#endif

namespace TreeWidzard {
namespace GPU {

#if CUDA_AVAILABLE
// CUDA kernel for parallel witness set computation
__global__ void computeWitnessSetsKernel(
    const int* bag_data,
    const int* bag_sizes,
    int* result_data,
    int* result_sizes,
    int num_bags,
    int max_bag_size,
    const char* core_name,
    int tree_width
) {
    int tid = blockIdx.x * blockDim.x + threadIdx.x;
    
    if (tid < num_bags) {
        // Each thread processes one bag
        const int* current_bag = bag_data + tid * max_bag_size;
        int bag_size = bag_sizes[tid];
        
        // Compute witness set for this bag
        // This is a simplified example - actual implementation would
        // depend on the specific DP core being used
        
        int* current_result = result_data + tid * max_bag_size;
        int result_size = 0;
        
        // Example: Simple witness computation
        for (int i = 0; i < bag_size; i++) {
            // Process vertex i in the bag
            current_result[result_size++] = current_bag[i];
        }
        
        result_sizes[tid] = result_size;
    }
}

// CUDA implementation
CUDAProcessor::CUDAProcessor(int device) 
    : cuda_context(nullptr), device_memory(nullptr), allocated_memory(0), device_id(device) {
}

CUDAProcessor::~CUDAProcessor() {
    cleanup();
}

bool CUDAProcessor::initialize() {
    if (!checkCUDAAvailability()) {
        std::cerr << "CUDA not available on this system" << std::endl;
        return false;
    }
    
    cudaError_t error = cudaSetDevice(device_id);
    if (error != cudaSuccess) {
        std::cerr << "Failed to set CUDA device: " << cudaGetErrorString(error) << std::endl;
        return false;
    }
    
    // Allocate initial GPU memory (e.g., 1GB)
    allocated_memory = 1024 * 1024 * 1024; // 1GB
    error = cudaMalloc(&device_memory, allocated_memory);
    if (error != cudaSuccess) {
        std::cerr << "Failed to allocate GPU memory: " << cudaGetErrorString(error) << std::endl;
        return false;
    }
    
    std::cout << "CUDA GPU initialized successfully on device " << device_id << std::endl;
    return true;
}

GPUComputationResult CUDAProcessor::processBatch(
    const std::vector<Bag>& bags,
    const std::string& core_name,
    int tree_width
) {
    PerformanceTimer timer;
    timer.start();
    
    GPUComputationResult result;
    result.success = false;
    
    if (bags.empty()) {
        return result;
    }
    
    // Prepare data for GPU transfer
    size_t num_bags = bags.size();
    size_t max_bag_size = 0;
    for (const auto& bag : bags) {
        max_bag_size = std::max(max_bag_size, bag.get_elements().size());
    }
    
    // Flatten bag data for GPU transfer
    std::vector<int> bag_data(num_bags * max_bag_size, -1);
    std::vector<int> bag_sizes(num_bags);
    
    for (size_t i = 0; i < num_bags; i++) {
        const auto& elements = bags[i].get_elements();
        bag_sizes[i] = elements.size();
        
        size_t j = 0;
        for (unsigned element : elements) {
            bag_data[i * max_bag_size + j] = element;
            j++;
        }
    }
    
    // Allocate GPU memory for input and output
    int* d_bag_data;
    int* d_bag_sizes;
    int* d_result_data;
    int* d_result_sizes;
    
    size_t bag_data_size = num_bags * max_bag_size * sizeof(int);
    size_t bag_sizes_size = num_bags * sizeof(int);
    
    cudaMalloc(&d_bag_data, bag_data_size);
    cudaMalloc(&d_bag_sizes, bag_sizes_size);
    cudaMalloc(&d_result_data, bag_data_size); // Same size for results
    cudaMalloc(&d_result_sizes, bag_sizes_size);
    
    // Transfer data to GPU
    cudaMemcpy(d_bag_data, bag_data.data(), bag_data_size, cudaMemcpyHostToDevice);
    cudaMemcpy(d_bag_sizes, bag_sizes.data(), bag_sizes_size, cudaMemcpyHostToDevice);
    
    // Launch kernel
    int block_size = 256;
    int grid_size = (num_bags + block_size - 1) / block_size;
    
    computeWitnessSetsKernel<<<grid_size, block_size>>>(
        d_bag_data, d_bag_sizes, d_result_data, d_result_sizes,
        num_bags, max_bag_size, core_name.c_str(), tree_width
    );
    
    // Wait for kernel completion
    cudaDeviceSynchronize();
    
    // Transfer results back
    std::vector<int> result_data(num_bags * max_bag_size);
    std::vector<int> result_sizes(num_bags);
    
    cudaMemcpy(result_data.data(), d_result_data, bag_data_size, cudaMemcpyDeviceToHost);
    cudaMemcpy(result_sizes.data(), d_result_sizes, bag_sizes_size, cudaMemcpyDeviceToHost);
    
    // Convert back to WitnessSet format
    result.witness_sets.reserve(num_bags);
    for (size_t i = 0; i < num_bags; i++) {
        WitnessSet witness_set;
        // Convert result data back to witness set
        // This is simplified - actual implementation would depend on witness structure
        result.witness_sets.push_back(std::move(witness_set));
    }
    
    // Cleanup GPU memory
    cudaFree(d_bag_data);
    cudaFree(d_bag_sizes);
    cudaFree(d_result_data);
    cudaFree(d_result_sizes);
    
    timer.stop();
    result.computation_time_ms = timer.getElapsedMilliseconds();
    result.memory_used_bytes = bag_data_size * 2 + bag_sizes_size * 2;
    result.success = true;
    
    return result;
}

bool CUDAProcessor::shouldUseGPU(size_t batch_size, const std::string& core_name) const {
    // Simple heuristic: use GPU for batch sizes > 100 and complex cores
    if (batch_size < 100) return false;
    
    // ChromaticNumber and CliqueNumber benefit most from GPU acceleration
    if (core_name.find("ChromaticNumber") != std::string::npos ||
        core_name.find("CliqueNumber") != std::string::npos) {
        return true;
    }
    
    return batch_size > 500; // For other cores, use higher threshold
}

bool CUDAProcessor::checkCUDAAvailability() const {
    int device_count = 0;
    cudaError_t error = cudaGetDeviceCount(&device_count);
    return (error == cudaSuccess) && (device_count > 0);
}

GPUDevice CUDAProcessor::getDeviceInfo() const {
    GPUDevice device;
    device.device_id = device_id;
    device.cuda_available = checkCUDAAvailability();
    device.opencl_available = false;
    
    if (device.cuda_available) {
        cudaDeviceProp prop;
        cudaGetDeviceProperties(&prop, device_id);
        
        device.name = prop.name;
        device.memory_mb = prop.totalGlobalMem / (1024 * 1024);
        device.compute_capability = prop.major * 10 + prop.minor;
    }
    
    return device;
}

void CUDAProcessor::cleanup() {
    if (device_memory) {
        cudaFree(device_memory);
        device_memory = nullptr;
    }
    allocated_memory = 0;
}

#else // No CUDA available

CUDAProcessor::CUDAProcessor(int device) : device_id(device) {}
CUDAProcessor::~CUDAProcessor() {}
bool CUDAProcessor::initialize() { return false; }
GPUComputationResult CUDAProcessor::processBatch(const std::vector<Bag>&, const std::string&, int) {
    return GPUComputationResult{};
}
bool CUDAProcessor::shouldUseGPU(size_t, const std::string&) const { return false; }
bool CUDAProcessor::checkCUDAAvailability() const { return false; }
GPUDevice CUDAProcessor::getDeviceInfo() const { return GPUDevice{}; }
void CUDAProcessor::cleanup() {}

#endif // CUDA_AVAILABLE

// OpenCL implementation (simplified)
OpenCLProcessor::OpenCLProcessor() 
    : opencl_context(nullptr), command_queue(nullptr), program(nullptr) {
}

OpenCLProcessor::~OpenCLProcessor() {
    cleanup();
}

bool OpenCLProcessor::initialize() {
#if OPENCL_AVAILABLE
    try {
        // Get platform
        std::vector<cl::Platform> platforms;
        cl::Platform::get(&platforms);
        
        if (platforms.empty()) {
            std::cerr << "No OpenCL platforms found" << std::endl;
            return false;
        }
        
        // Get devices
        std::vector<cl::Device> devices;
        platforms[0].getDevices(CL_DEVICE_TYPE_GPU, &devices);
        
        if (devices.empty()) {
            std::cerr << "No OpenCL GPU devices found" << std::endl;
            return false;
        }
        
        // Create context and command queue
        cl::Context* context = new cl::Context(devices);
        cl::CommandQueue* queue = new cl::CommandQueue(*context, devices[0]);
        
        opencl_context = context;
        command_queue = queue;
        
        std::cout << "OpenCL GPU initialized successfully" << std::endl;
        return true;
    } catch (const cl::Error& e) {
        std::cerr << "OpenCL error: " << e.what() << " (" << e.err() << ")" << std::endl;
        return false;
    }
#else
    std::cerr << "OpenCL not available at compile time" << std::endl;
    return false;
#endif
}

GPUComputationResult OpenCLProcessor::processBatch(
    const std::vector<Bag>& bags,
    const std::string& core_name,
    int tree_width
) {
    // Simplified OpenCL implementation
    GPUComputationResult result;
    result.success = false;
    
    // TODO: Implement OpenCL kernel execution
    // Similar to CUDA implementation but using OpenCL API
    
    return result;
}

bool OpenCLProcessor::shouldUseGPU(size_t batch_size, const std::string& core_name) const {
    return batch_size > 100; // Simple heuristic
}

GPUDevice OpenCLProcessor::getDeviceInfo() const {
    GPUDevice device;
    device.opencl_available = true;
    device.cuda_available = false;
    
    // TODO: Query actual OpenCL device properties
    
    return device;
}

void OpenCLProcessor::cleanup() {
    if (command_queue) {
        delete static_cast<cl::CommandQueue*>(command_queue);
        command_queue = nullptr;
    }
    if (opencl_context) {
        delete static_cast<cl::Context*>(opencl_context);
        opencl_context = nullptr;
    }
}

// GPU Manager implementation
GPUManager::GPUManager() : gpu_enabled(false) {
}

GPUManager::~GPUManager() {
    processors.clear();
}

bool GPUManager::initialize() {
    // Try to initialize CUDA first
    auto cuda_processor = std::make_unique<CUDAProcessor>();
    if (cuda_processor->initialize()) {
        processors.push_back(std::move(cuda_processor));
        gpu_enabled = true;
    }
    
    // Try OpenCL as fallback
    auto opencl_processor = std::make_unique<OpenCLProcessor>();
    if (opencl_processor->initialize()) {
        processors.push_back(std::move(opencl_processor));
        gpu_enabled = true;
    }
    
    if (processors.empty()) {
        std::cout << "No GPU processors available - falling back to CPU only" << std::endl;
        return false;
    }
    
    return selectOptimalDevice();
}

std::vector<GPUDevice> GPUManager::detectDevices() const {
    std::vector<GPUDevice> devices;
    
    for (const auto& processor : processors) {
        devices.push_back(processor->getDeviceInfo());
    }
    
    return devices;
}

bool GPUManager::selectOptimalDevice() {
    auto devices = detectDevices();
    
    if (devices.empty()) {
        return false;
    }
    
    // Select device with most memory and CUDA support if available
    auto best_device = std::max_element(devices.begin(), devices.end(),
        [](const GPUDevice& a, const GPUDevice& b) {
            if (a.cuda_available && !b.cuda_available) return false;
            if (!a.cuda_available && b.cuda_available) return true;
            return a.memory_mb < b.memory_mb;
        });
    
    selected_device = *best_device;
    
    std::cout << "Selected GPU: " << selected_device->name 
              << " (" << selected_device->memory_mb << " MB)" << std::endl;
    
    return true;
}

GPUComputationResult GPUManager::processWithAutoSelection(
    const std::vector<Bag>& bags,
    const std::string& core_name,
    int tree_width
) {
    if (!gpu_enabled || processors.empty()) {
        return GPUComputationResult{}; // Fall back to CPU
    }
    
    // Use first available processor
    auto& processor = processors[0];
    
    if (processor->shouldUseGPU(bags.size(), core_name)) {
        return processor->processBatch(bags, core_name, tree_width);
    }
    
    return GPUComputationResult{}; // Fall back to CPU
}

void GPUManager::printDeviceInfo() const {
    if (!selected_device.has_value()) {
        std::cout << "No GPU device selected" << std::endl;
        return;
    }
    
    const auto& device = selected_device.value();
    std::cout << "GPU Device Information:" << std::endl;
    std::cout << "  Name: " << device.name << std::endl;
    std::cout << "  Memory: " << device.memory_mb << " MB" << std::endl;
    std::cout << "  CUDA Available: " << (device.cuda_available ? "Yes" : "No") << std::endl;
    std::cout << "  OpenCL Available: " << (device.opencl_available ? "Yes" : "No") << std::endl;
    if (device.cuda_available) {
        std::cout << "  Compute Capability: " << device.compute_capability << std::endl;
    }
}

// Utility functions
namespace Utils {

bool hasNVIDIAGPU() {
#if CUDA_AVAILABLE
    int device_count = 0;
    cudaError_t error = cudaGetDeviceCount(&device_count);
    return (error == cudaSuccess) && (device_count > 0);
#else
    return false;
#endif
}

bool isGPUBeneficial(const std::string& core_name, size_t problem_size) {
    // ChromaticNumber and CliqueNumber are most suitable for GPU acceleration
    if (core_name.find("ChromaticNumber") != std::string::npos ||
        core_name.find("CliqueNumber") != std::string::npos) {
        return problem_size > 50; // Lower threshold for complex cores
    }
    
    return problem_size > 200; // Higher threshold for simpler cores
}

double estimateSpeedup(const std::string& core_name, size_t batch_size) {
    double base_speedup = std::log2(batch_size); // Logarithmic scaling
    
    // Core-specific multipliers based on computational complexity
    if (core_name.find("ChromaticNumber") != std::string::npos) {
        return base_speedup * 50.0; // High complexity
    } else if (core_name.find("CliqueNumber") != std::string::npos) {
        return base_speedup * 30.0; // High complexity
    } else if (core_name.find("VertexCount") != std::string::npos) {
        return base_speedup * 2.0;  // Low complexity
    }
    
    return base_speedup * 10.0; // Default
}

} // namespace Utils

}} // namespace TreeWidzard::GPU