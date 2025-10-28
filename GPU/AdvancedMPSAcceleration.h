#ifndef ADVANCED_MPS_ACCELERATION_H
#define ADVANCED_MPS_ACCELERATION_H

#ifdef __APPLE__
#import <Metal/Metal.h>
#import <MetalPerformanceShaders/MetalPerformanceShaders.h>
#import <Foundation/Foundation.h>
#endif

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include "../Kernel/Bag.h"
#include "../Kernel/State.h"

// Forward declarations
class MultiGraph;

// ChromaticWitness structure that mirrors the Metal shader
struct ChromaticWitness {
    unsigned int coloring[32];  // Support up to 32 vertices per bag
    unsigned int num_colors;    // Number of colors used
    unsigned int bag_size;      // Number of vertices in bag
    bool valid;                 // Whether this witness is valid
    
    ChromaticWitness() : num_colors(0), bag_size(0), valid(false) {
        for (int i = 0; i < 32; i++) coloring[i] = 0;
    }
};

// BagData structure for GPU computation
struct BagData {
    unsigned int vertices[32];  // Vertices in this bag
    unsigned int size;          // Number of vertices
    unsigned int max_colors;    // Maximum colors allowed
    
    BagData() : size(0), max_colors(3) {
        for (int i = 0; i < 32; i++) vertices[i] = 0;
    }
};

// EdgeData for representing graph edges
struct EdgeData {
    unsigned int from;
    unsigned int to;
    
    EdgeData(unsigned int f, unsigned int t) : from(f), to(t) {}
};

class AdvancedMPSDevice {
private:
#ifdef __APPLE__
    id<MTLDevice> device;
    id<MTLCommandQueue> command_queue;
    id<MTLLibrary> library;
    id<MTLFunction> chromatic_function;
    id<MTLFunction> union_function;
    id<MTLFunction> intro_vertex_function;
    id<MTLFunction> forget_vertex_function;
    id<MTLComputePipelineState> chromatic_pipeline;
    id<MTLComputePipelineState> union_pipeline;
    id<MTLComputePipelineState> intro_vertex_pipeline;
    id<MTLComputePipelineState> forget_vertex_pipeline;
#endif
    
    bool available;
    size_t max_memory;
    std::string device_name;
    
public:
    AdvancedMPSDevice();
    ~AdvancedMPSDevice();
    
    bool initialize();
    bool isAvailable() const { return available; }
    size_t getMaxMemory() const { return max_memory; }
    const std::string& getDeviceName() const { return device_name; }
    
    void printDeviceInfo() const;
    
    // Advanced witness computation methods
    std::vector<ChromaticWitness> computeChromaticWitnesses(
        const Bag& bag, 
        const std::vector<EdgeData>& edges,
        unsigned int max_colors = 3
    );
    
    std::vector<ChromaticWitness> unionWitnessSets(
        const std::vector<ChromaticWitness>& set_a,
        const std::vector<ChromaticWitness>& set_b
    );
    
    std::vector<ChromaticWitness> introVertexOperation(
        const std::vector<ChromaticWitness>& input_witnesses,
        unsigned int new_vertex,
        unsigned int max_colors = 3
    );
    
    std::vector<ChromaticWitness> forgetVertexOperation(
        const std::vector<ChromaticWitness>& input_witnesses,
        unsigned int forget_position
    );
    
    // Performance measurement
    struct PerformanceStats {
        double computation_time_ms;
        double memory_bandwidth_gbps;
        size_t witnesses_processed;
        size_t witnesses_generated;
    };
    
    PerformanceStats getLastPerformanceStats() const { return last_stats; }
    
    // CPU fallback method
    std::vector<ChromaticWitness> computeChromaticWitnessesCPU(
        const Bag& bag, 
        const std::vector<EdgeData>& edges,
        unsigned int max_colors = 3
    );
    
private:
    PerformanceStats last_stats;
    
#ifdef __APPLE__
    bool compileShaders();
    id<MTLBuffer> createBuffer(size_t size, const void* data = nullptr);
    void dispatchCompute(id<MTLComputePipelineState> pipeline, 
                        const std::vector<id<MTLBuffer>>& buffers,
                        unsigned int threads_per_group,
                        unsigned int num_groups);
#endif
};

class AdvancedMPSSearchStrategy {
private:
    std::shared_ptr<AdvancedMPSDevice> mps_device;
    bool use_gpu_acceleration;
    unsigned int max_colors;
    
    // Performance tracking
    struct SearchStats {
        size_t total_witnesses_computed;
        size_t total_gpu_operations;
        double total_gpu_time_ms;
        double total_cpu_fallback_time_ms;
        size_t bags_processed;
    };
    
    SearchStats stats;
    
public:
    AdvancedMPSSearchStrategy(unsigned int max_colors = 3);
    ~AdvancedMPSSearchStrategy();
    
    // Main search interface
    bool processTreeDecomposition(const std::vector<Bag>& bags, 
                                 const std::vector<EdgeData>& edges);
    
    // Individual DP operations with GPU acceleration
    std::vector<State> processIntroVertex(const std::vector<State>& states, 
                                        unsigned int vertex,
                                        const std::vector<EdgeData>& edges);
                                        
    std::vector<State> processIntroEdge(const std::vector<State>& states,
                                      unsigned int u, unsigned int v);
                                      
    std::vector<State> processForgetVertex(const std::vector<State>& states,
                                         unsigned int vertex);
                                         
    std::vector<State> processJoin(const std::vector<State>& left_states,
                                 const std::vector<State>& right_states);
    
    // Performance and statistics
    SearchStats getSearchStats() const { return stats; }
    void printPerformanceReport() const;
    
    // Configuration
    void setMaxColors(unsigned int colors) { max_colors = colors; }
    void enableGPUAcceleration(bool enable) { use_gpu_acceleration = enable; }
    bool isGPUAvailable() const { return mps_device && mps_device->isAvailable(); }
    
private:
    // Helper methods
    BagData bagToBagData(const Bag& bag) const;
    std::vector<ChromaticWitness> stateToWitnesses(const State& state) const;
    State witnessesToState(const std::vector<ChromaticWitness>& witnesses, 
                          const Bag& bag) const;
};

// Utility functions for TreeWidzard integration
namespace MPSUtils {
    // Convert TreeWidzard MultiGraph to EdgeData
    std::vector<EdgeData> multiGraphToEdges(const MultiGraph& graph);
    
    // Extract edges relevant to a specific bag
    std::vector<EdgeData> getRelevantEdges(const std::vector<EdgeData>& all_edges,
                                          const Bag& bag);
    
    // Performance comparison utilities
    struct BenchmarkResult {
        double cpu_time_ms;
        double gpu_time_ms;
        double speedup_factor;
        size_t witnesses_computed;
        bool gpu_successful;
    };
    
    BenchmarkResult benchmarkChromaticComputation(
        const Bag& bag,
        const std::vector<EdgeData>& edges,
        unsigned int max_colors = 3
    );
}

#endif // ADVANCED_MPS_ACCELERATION_H