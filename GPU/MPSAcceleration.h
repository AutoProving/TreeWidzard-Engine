#ifndef TREEWIDZARD_MPS_ACCELERATION_H
#define TREEWIDZARD_MPS_ACCELERATION_H

#ifdef __APPLE__
#include <Foundation/Foundation.h>
#include <Metal/Metal.h>
#include <MetalPerformanceShaders/MetalPerformanceShaders.h>
#endif

#include "../DPCores/Source/ChromaticNumber_AtMost.h"
#include "../DPCores/Source/VertexCount.h"
#include "../Kernel/Bag.h"
#include "../Kernel/State.h"
#include "../Kernel/Witness.h"
#include "../Kernel/WitnessSet.h"
#include <memory>
#include <string>
#include <vector>

namespace TreeWidzard {
namespace MPS {

/**
 * Metal Performance Shaders (MPS) acceleration for Apple Silicon
 *
 * Provides GPU acceleration using Apple's Metal framework on M-series chips.
 * This is the Apple Silicon equivalent of CUDA acceleration.
 */

#ifdef __APPLE__

class MPSDevice {
private:
  id<MTLDevice> device;
  id<MTLCommandQueue> command_queue;
  id<MTLLibrary> library;
  bool available;
  size_t max_memory;
  std::string device_name;

public:
  MPSDevice();
  ~MPSDevice();

  bool isAvailable() const { return available; }
  size_t getMaxMemory() const { return max_memory; }
  const std::string &getDeviceName() const { return device_name; }

  // Device management
  id<MTLDevice> getDevice() const { return device; }
  id<MTLCommandQueue> getCommandQueue() const { return command_queue; }
  id<MTLLibrary> getLibrary() const { return library; }

  // Memory management
  id<MTLBuffer>
  createBuffer(size_t size,
               MTLResourceOptions options = MTLResourceStorageModeShared);
  void synchronize();

  // Performance info
  void printDeviceInfo() const;
};

class MPSWitnessComputation {
private:
  std::shared_ptr<MPSDevice> device;
  id<MTLComputePipelineState> chromatic_pipeline;
  id<MTLComputePipelineState> clique_pipeline;
  id<MTLComputePipelineState> vertex_count_pipeline;
  id<MTLComputePipelineState> independent_set_pipeline;

public:
  MPSWitnessComputation(std::shared_ptr<MPSDevice> device);
  ~MPSWitnessComputation();

  // Initialize compute pipelines
  bool initialize();

  // Witness computations
  std::shared_ptr<WitnessSet> computeChromaticNumberWitnesses(const Bag &bag,
                                                              int max_colors);
  std::shared_ptr<WitnessSet> computeCliqueNumberWitnesses(
      const Bag &bag, const std::vector<std::pair<unsigned, unsigned>> &edges);
  std::shared_ptr<WitnessSet> computeVertexCountWitnesses(const Bag &bag);
  std::shared_ptr<WitnessSet> computeIndependentSetWitnesses(
      const Bag &bag, const std::vector<std::pair<unsigned, unsigned>> &edges);

  // Batch operations
  std::vector<std::shared_ptr<WitnessSet>>
  computeBatchWitnesses(const std::vector<Bag> &bags,
                        const std::string &property_type);
};

class MPSStateProcessor {
private:
  std::shared_ptr<MPSDevice> device;
  id<MTLComputePipelineState> intro_vertex_pipeline;
  id<MTLComputePipelineState> intro_edge_pipeline;
  id<MTLComputePipelineState> forget_vertex_pipeline;
  id<MTLComputePipelineState> join_pipeline;

public:
  MPSStateProcessor(std::shared_ptr<MPSDevice> device);
  ~MPSStateProcessor();

  bool initialize();

  // DP operations
  std::vector<State> processIntroVertex(const std::vector<State> &states,
                                        unsigned vertex);
  std::vector<State> processIntroEdge(const std::vector<State> &states,
                                      unsigned u, unsigned v);
  std::vector<State> processForgetVertex(const std::vector<State> &states,
                                         unsigned vertex);
  std::vector<State> processJoin(const std::vector<State> &left_states,
                                 const std::vector<State> &right_states);

  // Batch processing
  std::vector<State> processBatch(const std::vector<State> &states,
                                  const std::string &operation,
                                  const std::vector<unsigned> &parameters);
};

class MPSManager {
private:
  static std::shared_ptr<MPSManager> instance;
  std::shared_ptr<MPSDevice> device;
  std::unique_ptr<MPSWitnessComputation> witness_computer;
  std::unique_ptr<MPSStateProcessor> state_processor;
  bool initialized;

  MPSManager();

public:
  static std::shared_ptr<MPSManager> getInstance();

  bool initialize();
  bool isAvailable() const;

  // Accessors
  std::shared_ptr<MPSDevice> getDevice() const { return device; }
  MPSWitnessComputation *getWitnessComputer() const {
    return witness_computer.get();
  }
  MPSStateProcessor *getStateProcessor() const { return state_processor.get(); }

  // System information
  void printSystemInfo() const;
  bool checkCompatibility() const;

  // Performance monitoring
  void resetPerformanceCounters();
  void printPerformanceStats() const;
};

#else // Non-Apple platforms

// Stub implementations for non-Apple platforms
class MPSDevice {
public:
  bool isAvailable() const { return false; }
  void printDeviceInfo() const {}
};

class MPSManager {
public:
  static std::shared_ptr<MPSManager> getInstance() { return nullptr; }
  bool initialize() { return false; }
  bool isAvailable() const { return false; }
  void printSystemInfo() const {}
};

#endif // __APPLE__

} // namespace MPS
} // namespace TreeWidzard

#endif // TREEWIDZARD_MPS_ACCELERATION_H