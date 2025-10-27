#include <cuda_runtime.h>
#include <device_launch_parameters.h>
#include <cub/cub.cuh>

/**
 * CUDA kernels for dynamic programming operations in TreeWidzard
 * 
 * These kernels implement the core DP operations: intro_v, intro_e, forget_v, join
 * that are fundamental to tree decomposition-based algorithms.
 */

#include "witness_kernels.cu" // Include witness kernel definitions

// Additional constants
#define MAX_EDGES_PER_BAG 64
#define WARP_SIZE 32

// Data structures for DP operations
struct DPOperation {
    enum Type { INTRO_V, INTRO_E, FORGET_V, JOIN };
    Type type;
    int vertex_or_edge[2]; // vertex for intro_v/forget_v, edge endpoints for intro_e
    int bag_id;
};

struct DPState {
    GPUBag bag;
    GPUWitnessSet witness_set;
    int state_id;
    bool is_valid;
};

/**
 * CUDA kernel for introduce vertex operations
 * Processes multiple intro_v operations in parallel
 */
__global__ void introVertexKernel(
    const DPState* input_states,
    DPState* output_states,
    const DPOperation* operations,
    int num_operations,
    const char* core_name,
    int core_name_len
) {
    int tid = blockIdx.x * blockDim.x + threadIdx.x;
    
    if (tid >= num_operations) return;
    
    const DPOperation& op = operations[tid];
    if (op.type != DPOperation::INTRO_V) return;
    
    const DPState& input_state = input_states[tid];
    DPState& output_state = output_states[tid];
    
    // Copy input state
    output_state = input_state;
    output_state.state_id = tid;
    
    // Add new vertex to bag
    int new_vertex = op.vertex_or_edge[0];
    GPUBag& new_bag = output_state.bag;
    
    if (new_bag.size < MAX_BAG_SIZE) {
        new_bag.elements[new_bag.size] = new_vertex;
        new_bag.size++;
        
        // Recompute witness set for new bag
        recomputeWitnessSet(new_bag, output_state.witness_set, core_name, core_name_len);
        output_state.is_valid = true;
    } else {
        output_state.is_valid = false;
    }
}

/**
 * CUDA kernel for introduce edge operations
 */
__global__ void introEdgeKernel(
    const DPState* input_states,
    DPState* output_states,
    const DPOperation* operations,
    int num_operations,
    const char* core_name,
    int core_name_len
) {
    int tid = blockIdx.x * blockDim.x + threadIdx.x;
    
    if (tid >= num_operations) return;
    
    const DPOperation& op = operations[tid];
    if (op.type != DPOperation::INTRO_E) return;
    
    const DPState& input_state = input_states[tid];
    DPState& output_state = output_states[tid];
    
    // Copy input state
    output_state = input_state;
    output_state.state_id = tid;
    
    // Process edge introduction
    int vertex1 = op.vertex_or_edge[0];
    int vertex2 = op.vertex_or_edge[1];
    
    // Update witness set based on new edge
    updateWitnessSetForEdge(output_state.bag, output_state.witness_set, 
                           vertex1, vertex2, core_name, core_name_len);
    output_state.is_valid = true;
}

/**
 * CUDA kernel for forget vertex operations
 */
__global__ void forgetVertexKernel(
    const DPState* input_states,
    DPState* output_states,
    const DPOperation* operations,
    int num_operations,
    const char* core_name,
    int core_name_len
) {
    int tid = blockIdx.x * blockDim.x + threadIdx.x;
    
    if (tid >= num_operations) return;
    
    const DPOperation& op = operations[tid];
    if (op.type != DPOperation::FORGET_V) return;
    
    const DPState& input_state = input_states[tid];
    DPState& output_state = output_states[tid];
    
    // Copy input state
    output_state = input_state;
    output_state.state_id = tid;
    
    // Remove vertex from bag
    int vertex_to_forget = op.vertex_or_edge[0];
    GPUBag& new_bag = output_state.bag;
    
    // Find and remove vertex
    bool found = false;
    for (int i = 0; i < new_bag.size; i++) {
        if (new_bag.elements[i] == vertex_to_forget) {
            // Shift elements to remove vertex
            for (int j = i; j < new_bag.size - 1; j++) {
                new_bag.elements[j] = new_bag.elements[j + 1];
            }
            new_bag.size--;
            found = true;
            break;
        }
    }
    
    if (found) {
        // Project witness set to new bag
        projectWitnessSet(new_bag, output_state.witness_set, vertex_to_forget, 
                         core_name, core_name_len);
        output_state.is_valid = true;
    } else {
        output_state.is_valid = false;
    }
}

/**
 * CUDA kernel for join operations
 */
__global__ void joinStatesKernel(
    const DPState* left_states,
    const DPState* right_states,
    DPState* output_states,
    int num_operations,
    const char* core_name,
    int core_name_len
) {
    int tid = blockIdx.x * blockDim.x + threadIdx.x;
    
    if (tid >= num_operations) return;
    
    const DPState& left_state = left_states[tid];
    const DPState& right_state = right_states[tid];
    DPState& output_state = output_states[tid];
    
    output_state.state_id = tid;
    
    // Bags should be identical for join operation
    bool bags_match = true;
    if (left_state.bag.size != right_state.bag.size) {
        bags_match = false;
    } else {
        for (int i = 0; i < left_state.bag.size; i++) {
            if (left_state.bag.elements[i] != right_state.bag.elements[i]) {
                bags_match = false;
                break;
            }
        }
    }
    
    if (bags_match) {
        output_state.bag = left_state.bag;
        
        // Join witness sets
        joinWitnessSetsDP(left_state.witness_set, right_state.witness_set,
                         output_state.witness_set, core_name, core_name_len);
        output_state.is_valid = true;
    } else {
        output_state.is_valid = false;
    }
}

/**
 * Parallel reduction kernel for combining DP results
 */
__global__ void reduceResults(
    const DPState* input_states,
    DPState* output_states,
    int num_states,
    int reduction_factor
) {
    int tid = blockIdx.x * blockDim.x + threadIdx.x;
    int output_id = tid / reduction_factor;
    
    if (tid >= num_states || output_id >= num_states / reduction_factor) return;
    
    // Use shared memory for efficient reduction
    __shared__ DPState shared_states[THREADS_PER_BLOCK];
    
    // Load data into shared memory
    if (threadIdx.x < THREADS_PER_BLOCK && tid < num_states) {
        shared_states[threadIdx.x] = input_states[tid];
    }
    
    __syncthreads();
    
    // Perform reduction
    for (int stride = reduction_factor / 2; stride > 0; stride /= 2) {
        if (threadIdx.x % (stride * 2) == 0 && threadIdx.x + stride < THREADS_PER_BLOCK) {
            // Combine states (simplified - depends on specific operation)
            combineStates(shared_states[threadIdx.x], shared_states[threadIdx.x + stride]);
        }
        __syncthreads();
    }
    
    // Write result
    if (threadIdx.x == 0) {
        output_states[output_id] = shared_states[0];
    }
}

/**
 * CUDA kernel for batch state validation
 */
__global__ void validateStatesKernel(
    DPState* states,
    bool* validation_results,
    int num_states,
    const char* core_name,
    int core_name_len
) {
    int tid = blockIdx.x * blockDim.x + threadIdx.x;
    
    if (tid >= num_states) return;
    
    DPState& state = states[tid];
    
    // Validate state based on core-specific rules
    bool is_valid = true;
    
    // Check bag validity
    if (state.bag.size <= 0 || state.bag.size > MAX_BAG_SIZE) {
        is_valid = false;
    }
    
    // Check witness set validity
    if (state.witness_set.count <= 0 || state.witness_set.count > MAX_WITNESSES_PER_BAG) {
        is_valid = false;
    }
    
    // Core-specific validation
    if (is_valid) {
        is_valid = validateStateForCore(state, core_name, core_name_len);
    }
    
    validation_results[tid] = is_valid;
    state.is_valid = is_valid;
}

// Device helper functions
__device__ void recomputeWitnessSet(
    const GPUBag& bag,
    GPUWitnessSet& witness_set,
    const char* core_name,
    int core_name_len
) {
    // Recompute witness set for the given bag and core
    witness_set.bag_id = bag.bag_id;
    witness_set.count = 0;
    
    // Core-specific witness computation
    if (strncmp(core_name, "ChromaticNumber", core_name_len) == 0) {
        // Recompute chromatic number witnesses
        computeChromaticWitnesses(bag, witness_set);
    } else if (strncmp(core_name, "CliqueNumber", core_name_len) == 0) {
        // Recompute clique number witnesses
        computeCliqueWitnesses(bag, witness_set);
    } else if (strncmp(core_name, "VertexCount", core_name_len) == 0) {
        // Recompute vertex count witnesses
        computeVertexCountWitnesses(bag, witness_set);
    }
    // Add more cores as needed
}

__device__ void updateWitnessSetForEdge(
    const GPUBag& bag,
    GPUWitnessSet& witness_set,
    int vertex1,
    int vertex2,
    const char* core_name,
    int core_name_len
) {
    // Update witness set when a new edge is introduced
    // Filter out witnesses that become invalid due to the new edge
    
    int valid_count = 0;
    GPUWitness temp_witnesses[MAX_WITNESSES_PER_BAG];
    
    for (int i = 0; i < witness_set.count; i++) {
        if (isWitnessValidWithEdge(witness_set.witnesses[i], vertex1, vertex2, core_name)) {
            temp_witnesses[valid_count] = witness_set.witnesses[i];
            valid_count++;
        }
    }
    
    // Copy back valid witnesses
    witness_set.count = valid_count;
    for (int i = 0; i < valid_count; i++) {
        witness_set.witnesses[i] = temp_witnesses[i];
    }
}

__device__ void projectWitnessSet(
    const GPUBag& new_bag,
    GPUWitnessSet& witness_set,
    int forgotten_vertex,
    const char* core_name,
    int core_name_len
) {
    // Project witness set when forgetting a vertex
    // Remove information about the forgotten vertex from witnesses
    
    for (int i = 0; i < witness_set.count; i++) {
        GPUWitness& witness = witness_set.witnesses[i];
        
        // Remove forgotten vertex from witness data
        int new_size = 0;
        for (int j = 0; j < witness.size; j++) {
            if (witness.data[j] != forgotten_vertex) {
                witness.data[new_size] = witness.data[j];
                new_size++;
            }
        }
        witness.size = new_size;
    }
}

__device__ void joinWitnessSetsDP(
    const GPUWitnessSet& left,
    const GPUWitnessSet& right,
    GPUWitnessSet& result,
    const char* core_name,
    int core_name_len
) {
    result.bag_id = left.bag_id;
    result.count = 0;
    
    // Join compatible witnesses from left and right sets
    for (int i = 0; i < left.count && result.count < MAX_WITNESSES_PER_BAG; i++) {
        for (int j = 0; j < right.count && result.count < MAX_WITNESSES_PER_BAG; j++) {
            if (areWitnessesCompatible(left.witnesses[i], right.witnesses[j], core_name)) {
                GPUWitness joined;
                if (joinTwoWitnesses(left.witnesses[i], right.witnesses[j], joined, core_name)) {
                    result.witnesses[result.count] = joined;
                    result.count++;
                }
            }
        }
    }
}

__device__ void combineStates(DPState& target, const DPState& source) {
    // Combine two states during reduction
    // This is a simplified combination - real implementation depends on semantics
    
    if (!source.is_valid) return;
    
    if (!target.is_valid) {
        target = source;
        return;
    }
    
    // Merge witness sets (keeping the one with more witnesses for simplicity)
    if (source.witness_set.count > target.witness_set.count) {
        target.witness_set = source.witness_set;
    }
}

__device__ bool validateStateForCore(
    const DPState& state,
    const char* core_name,
    int core_name_len
) {
    // Core-specific state validation
    
    if (strncmp(core_name, "ChromaticNumber", core_name_len) == 0) {
        return validateChromaticState(state);
    } else if (strncmp(core_name, "CliqueNumber", core_name_len) == 0) {
        return validateCliqueState(state);
    } else if (strncmp(core_name, "VertexCount", core_name_len) == 0) {
        return validateVertexCountState(state);
    }
    
    return true; // Default: assume valid
}

// Additional helper functions (simplified implementations)
__device__ void computeChromaticWitnesses(const GPUBag& bag, GPUWitnessSet& witness_set) {
    // Simplified chromatic number witness computation
    witness_set.count = 1;
    witness_set.witnesses[0].size = bag.size;
    witness_set.witnesses[0].valid = true;
    
    for (int i = 0; i < bag.size; i++) {
        witness_set.witnesses[0].data[i] = i % 3; // Simple 3-coloring
    }
}

__device__ void computeCliqueWitnesses(const GPUBag& bag, GPUWitnessSet& witness_set) {
    // Simplified clique witness computation
    witness_set.count = 1;
    witness_set.witnesses[0].size = bag.size;
    witness_set.witnesses[0].valid = true;
    
    for (int i = 0; i < bag.size; i++) {
        witness_set.witnesses[0].data[i] = bag.elements[i];
    }
}

__device__ void computeVertexCountWitnesses(const GPUBag& bag, GPUWitnessSet& witness_set) {
    // Simple vertex count witness
    witness_set.count = 1;
    witness_set.witnesses[0].size = 1;
    witness_set.witnesses[0].data[0] = bag.size;
    witness_set.witnesses[0].valid = true;
}

__device__ bool isWitnessValidWithEdge(
    const GPUWitness& witness,
    int vertex1,
    int vertex2,
    const char* core_name
) {
    // Check if witness remains valid with new edge
    // Simplified implementation
    return true;
}

__device__ bool areWitnessesCompatible(
    const GPUWitness& left,
    const GPUWitness& right,
    const char* core_name
) {
    // Check if two witnesses are compatible for joining
    if (left.size != right.size) return false;
    
    for (int i = 0; i < left.size; i++) {
        if (left.data[i] != right.data[i]) return false;
    }
    
    return true;
}

__device__ bool joinTwoWitnesses(
    const GPUWitness& left,
    const GPUWitness& right,
    GPUWitness& result,
    const char* core_name
) {
    // Join two compatible witnesses
    result = left; // Simple copy for demonstration
    result.valid = left.valid && right.valid;
    return result.valid;
}

__device__ bool validateChromaticState(const DPState& state) {
    // Validate chromatic number state
    return state.witness_set.count > 0;
}

__device__ bool validateCliqueState(const DPState& state) {
    // Validate clique number state
    return state.witness_set.count > 0;
}

__device__ bool validateVertexCountState(const DPState& state) {
    // Validate vertex count state
    return state.witness_set.count == 1;
}

// Host functions for kernel launching
extern "C" {

cudaError_t launchIntroVertexKernel(
    const DPState* d_input_states,
    DPState* d_output_states,
    const DPOperation* d_operations,
    int num_operations,
    const char* core_name,
    cudaStream_t stream = 0
) {
    int grid_size = (num_operations + THREADS_PER_BLOCK - 1) / THREADS_PER_BLOCK;
    int core_name_len = strlen(core_name);
    
    introVertexKernel<<<grid_size, THREADS_PER_BLOCK, 0, stream>>>(
        d_input_states, d_output_states, d_operations, num_operations, core_name, core_name_len
    );
    
    return cudaGetLastError();
}

cudaError_t launchIntroEdgeKernel(
    const DPState* d_input_states,
    DPState* d_output_states,
    const DPOperation* d_operations,
    int num_operations,
    const char* core_name,
    cudaStream_t stream = 0
) {
    int grid_size = (num_operations + THREADS_PER_BLOCK - 1) / THREADS_PER_BLOCK;
    int core_name_len = strlen(core_name);
    
    introEdgeKernel<<<grid_size, THREADS_PER_BLOCK, 0, stream>>>(
        d_input_states, d_output_states, d_operations, num_operations, core_name, core_name_len
    );
    
    return cudaGetLastError();
}

cudaError_t launchForgetVertexKernel(
    const DPState* d_input_states,
    DPState* d_output_states,
    const DPOperation* d_operations,
    int num_operations,
    const char* core_name,
    cudaStream_t stream = 0
) {
    int grid_size = (num_operations + THREADS_PER_BLOCK - 1) / THREADS_PER_BLOCK;
    int core_name_len = strlen(core_name);
    
    forgetVertexKernel<<<grid_size, THREADS_PER_BLOCK, 0, stream>>>(
        d_input_states, d_output_states, d_operations, num_operations, core_name, core_name_len
    );
    
    return cudaGetLastError();
}

cudaError_t launchJoinStatesKernel(
    const DPState* d_left_states,
    const DPState* d_right_states,
    DPState* d_output_states,
    int num_operations,
    const char* core_name,
    cudaStream_t stream = 0
) {
    int grid_size = (num_operations + THREADS_PER_BLOCK - 1) / THREADS_PER_BLOCK;
    int core_name_len = strlen(core_name);
    
    joinStatesKernel<<<grid_size, THREADS_PER_BLOCK, 0, stream>>>(
        d_left_states, d_right_states, d_output_states, num_operations, core_name, core_name_len
    );
    
    return cudaGetLastError();
}

} // extern "C"