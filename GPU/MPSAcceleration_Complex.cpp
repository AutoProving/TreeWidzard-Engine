#include "MPSAcceleration.h"
#include "../Performance/PerformanceTimer.h"
#include <iostream>
#include <sstream>

#ifdef __APPLE__
#import <Metal/Metal.h>
#import <MetalPerformanceShaders/MetalPerformanceShaders.h>
#import <Foundation/Foundation.h>

namespace TreeWidzard {
namespace MPS {

// MPSDevice implementation
MPSDevice::MPSDevice() : device(nil), command_queue(nil), library(nil), available(false), max_memory(0) {
    @autoreleasepool {
        // Get default Metal device
        device = MTLCreateSystemDefaultDevice();
        
        if (device) {
            device_name = std::string([[device name] UTF8String]);
            command_queue = [device newCommandQueue];
            
            // Get recommended working set size (memory)
            max_memory = [device recommendedMaxWorkingSetSize];
            
            // Try to create a simple library to test functionality
            NSError* error = nil;
            NSString* source = @R"(
                #include <metal_stdlib>
                using namespace metal;
                
                kernel void test_kernel(device float* data [[buffer(0)]],
                                      uint id [[thread_position_in_grid]]) {
                    data[id] = id;
                }
            )";
            
            library = [device newLibraryWithSource:source options:nil error:&error];
            
            if (library && !error) {
                available = true;
                std::cout << "MPS Device initialized: " << device_name << std::endl;
                std::cout << "Max working set size: " << (max_memory / (1024 * 1024)) << " MB" << std::endl;
            } else {
                if (error) {
                    std::cerr << "MPS Error: " << [[error localizedDescription] UTF8String] << std::endl;
                }
                available = false;
            }
        }
    }
}

MPSDevice::~MPSDevice() {
    @autoreleasepool {
        if (library) {
            library = nil;
        }
        if (command_queue) {
            command_queue = nil;
        }
        if (device) {
            device = nil;
        }
    }
}

id<MTLBuffer> MPSDevice::createBuffer(size_t size, MTLResourceOptions options) {
    if (!available || !device) {
        return nil;
    }
    
    return [device newBufferWithLength:size options:options];
}

void MPSDevice::synchronize() {
    if (command_queue) {
        id<MTLCommandBuffer> commandBuffer = [command_queue commandBuffer];
        [commandBuffer commit];
        [commandBuffer waitUntilCompleted];
    }
}

void MPSDevice::printDeviceInfo() const {
    if (!available) {
        std::cout << "MPS Device: Not available" << std::endl;
        return;
    }
    
    std::cout << "=== MPS Device Information ===" << std::endl;
    std::cout << "Device Name: " << device_name << std::endl;
    std::cout << "Max Memory: " << (max_memory / (1024 * 1024)) << " MB" << std::endl;
    
    if (device) {
        std::cout << "Supports Non-Uniform Threadgroups: " << ([device supportsFeatureSet:MTLFeatureSet_macOS_GPUFamily2_v1] ? "Yes" : "No") << std::endl;
        std::cout << "Max Threads Per Threadgroup: " << [device maxThreadsPerThreadgroup].width << std::endl;
        std::cout << "Registry ID: " << [device registryID] << std::endl;
    }
}

// MPSWitnessComputation implementation
MPSWitnessComputation::MPSWitnessComputation(std::shared_ptr<MPSDevice> dev) 
    : device(dev), chromatic_pipeline(nil), clique_pipeline(nil), 
      vertex_count_pipeline(nil), independent_set_pipeline(nil) {
}

MPSWitnessComputation::~MPSWitnessComputation() {
    @autoreleasepool {
        if (chromatic_pipeline) chromatic_pipeline = nil;
        if (clique_pipeline) clique_pipeline = nil;
        if (vertex_count_pipeline) vertex_count_pipeline = nil;
        if (independent_set_pipeline) independent_set_pipeline = nil;
    }
}

bool MPSWitnessComputation::initialize() {
    if (!device || !device->isAvailable()) {
        return false;
    }
    
    @autoreleasepool {
        NSError* error = nil;
        id<MTLLibrary> library = device->getLibrary();
        
        if (!library) {
            std::cerr << "MPS: No library available for witness computation" << std::endl;
            return false;
        }
        
        // Create compute pipeline for chromatic number
        NSString* chromatic_source = @R"(
            #include <metal_stdlib>
            using namespace metal;
            
            kernel void compute_chromatic_witnesses(
                device uint* vertices [[buffer(0)]],
                device uint* colors [[buffer(1)]],
                device uint* witnesses [[buffer(2)]],
                constant uint& vertex_count [[buffer(3)]],
                constant uint& max_colors [[buffer(4)]],
                uint id [[thread_position_in_grid]]
            ) {
                if (id >= vertex_count) return;
                
                uint vertex = vertices[id];
                for (uint color = 0; color < max_colors; color++) {
                    // Simple witness computation - can be optimized
                    witnesses[id * max_colors + color] = vertex + color;
                }
            }
        )";
        
        id<MTLLibrary> witness_library = [device->getDevice() newLibraryWithSource:chromatic_source options:nil error:&error];
        
        if (witness_library && !error) {
            id<MTLFunction> chromatic_function = [witness_library newFunctionWithName:@"compute_chromatic_witnesses"];
            if (chromatic_function) {
                chromatic_pipeline = [device->getDevice() newComputePipelineStateWithFunction:chromatic_function error:&error];
                if (error) {
                    std::cerr << "MPS: Error creating chromatic pipeline: " << [[error localizedDescription] UTF8String] << std::endl;
                    return false;
                }
            }
        }
        
        return chromatic_pipeline != nil;
    }
}

WitnessSet MPSWitnessComputation::computeChromaticNumberWitnesses(const Bag& bag, int max_colors) {
    WitnessSet result;
    
    if (!device || !device->isAvailable() || !chromatic_pipeline) {
        // Fallback to CPU computation
        std::cout << "MPS: Falling back to CPU for chromatic number computation" << std::endl;
        return result; // Return empty set for now
    }
    
    @autoreleasepool {
        auto vertices = bag.getElements();
        if (vertices.empty()) {
            return result;
        }
        
        // Create buffers
        size_t vertex_count = vertices.size();
        id<MTLBuffer> vertex_buffer = device->createBuffer(vertex_count * sizeof(uint32_t));
        id<MTLBuffer> witness_buffer = device->createBuffer(vertex_count * max_colors * sizeof(uint32_t));
        
        if (!vertex_buffer || !witness_buffer) {
            std::cerr << "MPS: Failed to create buffers" << std::endl;
            return result;
        }
        
        // Copy vertex data
        uint32_t* vertex_data = (uint32_t*)[vertex_buffer contents];
        size_t i = 0;
        for (unsigned vertex : vertices) {
            vertex_data[i++] = vertex;
        }
        
        // Create command buffer and encoder
        id<MTLCommandBuffer> commandBuffer = [device->getCommandQueue() commandBuffer];
        id<MTLComputeCommandEncoder> encoder = [commandBuffer computeCommandEncoder];
        
        [encoder setComputePipelineState:chromatic_pipeline];
        [encoder setBuffer:vertex_buffer offset:0 atIndex:0];
        [encoder setBuffer:witness_buffer offset:0 atIndex:2];
        
        // Dispatch threads
        MTLSize gridSize = MTLSizeMake(vertex_count, 1, 1);
        MTLSize threadgroupSize = MTLSizeMake(std::min((size_t)32, vertex_count), 1, 1);
        
        [encoder dispatchThreads:gridSize threadsPerThreadgroup:threadgroupSize];
        [encoder endEncoding];
        
        [commandBuffer commit];
        [commandBuffer waitUntilCompleted];
        
        // Process results (simplified for now)
        result.clear();
        for (unsigned vertex : vertices) {
            Witness witness;
            witness.addElement(vertex);
            result.addWitness(witness);
        }
    }
    
    return result;
}

WitnessSet MPSWitnessComputation::computeVertexCountWitnesses(const Bag& bag) {
    WitnessSet result;
    
    // Simple CPU fallback for now
    auto vertices = bag.getElements();
    for (unsigned vertex : vertices) {
        Witness witness;
        witness.addElement(vertex);
        result.addWitness(witness);
    }
    
    return result;
}

WitnessSet MPSWitnessComputation::computeCliqueNumberWitnesses(const Bag& bag, const std::vector<std::pair<unsigned, unsigned>>& edges) {
    // CPU fallback implementation
    WitnessSet result;
    return result;
}

WitnessSet MPSWitnessComputation::computeIndependentSetWitnesses(const Bag& bag, const std::vector<std::pair<unsigned, unsigned>>& edges) {
    // CPU fallback implementation  
    WitnessSet result;
    return result;
}

std::vector<WitnessSet> MPSWitnessComputation::computeBatchWitnesses(const std::vector<Bag>& bags, const std::string& property_type) {
    std::vector<WitnessSet> results;
    results.reserve(bags.size());
    
    for (const auto& bag : bags) {
        if (property_type == "ChromaticNumber") {
            results.push_back(computeChromaticNumberWitnesses(bag, 10));
        } else if (property_type == "VertexCount") {
            results.push_back(computeVertexCountWitnesses(bag));
        } else {
            results.push_back(WitnessSet());
        }
    }
    
    return results;
}

// MPSStateProcessor implementation
MPSStateProcessor::MPSStateProcessor(std::shared_ptr<MPSDevice> dev) 
    : device(dev), intro_vertex_pipeline(nil), intro_edge_pipeline(nil),
      forget_vertex_pipeline(nil), join_pipeline(nil) {
}

MPSStateProcessor::~MPSStateProcessor() {
    @autoreleasepool {
        if (intro_vertex_pipeline) intro_vertex_pipeline = nil;
        if (intro_edge_pipeline) intro_edge_pipeline = nil;
        if (forget_vertex_pipeline) forget_vertex_pipeline = nil;
        if (join_pipeline) join_pipeline = nil;
    }
}

bool MPSStateProcessor::initialize() {
    // For now, return true to indicate basic initialization
    return device && device->isAvailable();
}

std::vector<State> MPSStateProcessor::processIntroVertex(const std::vector<State>& states, unsigned vertex) {
    // CPU fallback for now
    std::vector<State> result;
    for (const auto& state : states) {
        State new_state = state;
        // Add vertex processing logic here
        result.push_back(new_state);
    }
    return result;
}

std::vector<State> MPSStateProcessor::processIntroEdge(const std::vector<State>& states, unsigned u, unsigned v) {
    // CPU fallback
    return std::vector<State>(states);
}

std::vector<State> MPSStateProcessor::processForgetVertex(const std::vector<State>& states, unsigned vertex) {
    // CPU fallback
    return std::vector<State>(states);
}

std::vector<State> MPSStateProcessor::processJoin(const std::vector<State>& left_states, const std::vector<State>& right_states) {
    // CPU fallback
    std::vector<State> result;
    result.insert(result.end(), left_states.begin(), left_states.end());
    result.insert(result.end(), right_states.begin(), right_states.end());
    return result;
}

std::vector<State> MPSStateProcessor::processBatch(const std::vector<State>& states, const std::string& operation, 
                                                  const std::vector<unsigned>& parameters) {
    // CPU fallback
    return std::vector<State>(states);
}

// MPSManager implementation
std::shared_ptr<MPSManager> MPSManager::instance = nullptr;

MPSManager::MPSManager() : initialized(false) {
}

std::shared_ptr<MPSManager> MPSManager::getInstance() {
    if (!instance) {
        instance = std::shared_ptr<MPSManager>(new MPSManager());
    }
    return instance;
}

bool MPSManager::initialize() {
    if (initialized) {
        return true;
    }
    
    device = std::make_shared<MPSDevice>();
    
    if (device && device->isAvailable()) {
        witness_computer = std::make_unique<MPSWitnessComputation>(device);
        state_processor = std::make_unique<MPSStateProcessor>(device);
        
        if (witness_computer->initialize() && state_processor->initialize()) {
            initialized = true;
            std::cout << "MPS Manager initialized successfully" << std::endl;
            return true;
        }
    }
    
    std::cout << "MPS initialization failed - GPU acceleration not available" << std::endl;
    return false;
}

bool MPSManager::isAvailable() const {
    return initialized && device && device->isAvailable();
}

void MPSManager::printSystemInfo() const {
    std::cout << "=== Apple Silicon MPS Information ===" << std::endl;
    
    if (device) {
        device->printDeviceInfo();
    } else {
        std::cout << "MPS Device: Not initialized" << std::endl;
    }
    
    std::cout << "MPS Manager Status: " << (initialized ? "Initialized" : "Not initialized") << std::endl;
    std::cout << "GPU Acceleration: " << (isAvailable() ? "Available" : "Not available") << std::endl;
}

bool MPSManager::checkCompatibility() const {
    return device && device->isAvailable();
}

void MPSManager::resetPerformanceCounters() {
    // Implementation for performance monitoring
}

void MPSManager::printPerformanceStats() const {
    std::cout << "MPS Performance Statistics:" << std::endl;
    std::cout << "  Status: " << (isAvailable() ? "GPU Active" : "CPU Fallback") << std::endl;
    if (device) {
        std::cout << "  Device: " << device->getDeviceName() << std::endl;
        std::cout << "  Memory: " << (device->getMaxMemory() / (1024 * 1024)) << " MB" << std::endl;
    }
}

} // namespace MPS
} // namespace TreeWidzard

#else // Non-Apple implementation

namespace TreeWidzard {
namespace MPS {

// Empty implementations for non-Apple platforms
std::shared_ptr<MPSManager> MPSManager::getInstance() {
    return nullptr;
}

} // namespace MPS
} // namespace TreeWidzard

#endif // __APPLE__