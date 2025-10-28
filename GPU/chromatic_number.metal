#include <metal_stdlib>
using namespace metal;

// Data structures for GPU witness computation
struct ChromaticWitness {
    uint coloring[32];     // Support up to 32 vertices per bag
    uint num_colors;       // Number of colors used
    uint bag_size;         // Number of vertices in bag
    bool valid;            // Whether this witness is valid
};

struct BagData {
    uint vertices[32];     // Vertices in this bag
    uint size;             // Number of vertices
    uint max_colors;       // Maximum colors allowed
};

struct EdgeData {
    uint from;
    uint to;
};

// GPU kernel for chromatic number witness computation
kernel void compute_chromatic_witnesses(
    device const BagData& bag [[buffer(0)]],
    device const EdgeData* edges [[buffer(1)]],
    device const uint& num_edges [[buffer(2)]],
    device ChromaticWitness* witnesses [[buffer(3)]],
    device uint& witness_count [[buffer(4)]],
    uint thread_id [[thread_position_in_grid]]
) {
    // Each thread explores a different coloring
    if (thread_id >= (1u << (bag.size * 2))) return; // 4^bag_size colorings max
    
    ChromaticWitness witness;
    witness.bag_size = bag.size;
    witness.valid = true;
    witness.num_colors = 0;
    
    // Decode thread_id into a coloring
    uint coloring_id = thread_id;
    uint max_color = 0;
    
    for (uint i = 0; i < bag.size; i++) {
        uint color = coloring_id % bag.max_colors;
        coloring_id /= bag.max_colors;
        
        witness.coloring[i] = color;
        max_color = max(max_color, color);
    }
    
    witness.num_colors = max_color + 1;
    
    // Check if coloring is valid (no adjacent vertices have same color)
    for (uint edge_idx = 0; edge_idx < num_edges; edge_idx++) {
        uint from_vertex = edges[edge_idx].from;
        uint to_vertex = edges[edge_idx].to;
        
        // Find positions of vertices in bag
        int from_pos = -1, to_pos = -1;
        for (uint i = 0; i < bag.size; i++) {
            if (bag.vertices[i] == from_vertex) from_pos = i;
            if (bag.vertices[i] == to_vertex) to_pos = i;
        }
        
        // If both vertices are in bag and have same color, invalid
        if (from_pos >= 0 && to_pos >= 0 && 
            witness.coloring[from_pos] == witness.coloring[to_pos]) {
            witness.valid = false;
            break;
        }
    }
    
    // Only store valid witnesses
    if (witness.valid) {
        uint idx = atomic_fetch_add_explicit(&witness_count, 1u, memory_order_relaxed);
        if (idx < 10000) { // Limit to prevent overflow
            witnesses[idx] = witness;
        }
    }
}

// GPU kernel for witness set operations (union, intersection)
kernel void union_witness_sets(
    device const ChromaticWitness* set_a [[buffer(0)]],
    device const uint& count_a [[buffer(1)]],
    device const ChromaticWitness* set_b [[buffer(2)]],
    device const uint& count_b [[buffer(3)]],
    device ChromaticWitness* result [[buffer(4)]],
    device uint& result_count [[buffer(5)]],
    uint thread_id [[thread_position_in_grid]]
) {
    if (thread_id == 0) {
        result_count = 0;
        
        // Copy all witnesses from set_a
        for (uint i = 0; i < count_a; i++) {
            result[result_count++] = set_a[i];
        }
        
        // Add witnesses from set_b that are not already present
        for (uint i = 0; i < count_b; i++) {
            bool found = false;
            for (uint j = 0; j < count_a; j++) {
                // Simple equality check (can be optimized)
                if (set_a[j].num_colors == set_b[i].num_colors &&
                    set_a[j].bag_size == set_b[i].bag_size) {
                    bool equal = true;
                    for (uint k = 0; k < set_b[i].bag_size; k++) {
                        if (set_a[j].coloring[k] != set_b[i].coloring[k]) {
                            equal = false;
                            break;
                        }
                    }
                    if (equal) {
                        found = true;
                        break;
                    }
                }
            }
            if (!found && result_count < 10000) {
                result[result_count++] = set_b[i];
            }
        }
    }
}

// GPU kernel for intro vertex operation
kernel void intro_vertex_operation(
    device const ChromaticWitness* input_witnesses [[buffer(0)]],
    device const uint& input_count [[buffer(1)]],
    device const uint& new_vertex [[buffer(2)]],
    device const uint& max_colors [[buffer(3)]],
    device ChromaticWitness* output_witnesses [[buffer(4)]],
    device uint& output_count [[buffer(5)]],
    uint thread_id [[thread_position_in_grid]]
) {
    if (thread_id >= input_count * max_colors) return;
    
    uint witness_idx = thread_id / max_colors;
    uint new_color = thread_id % max_colors;
    
    ChromaticWitness input = input_witnesses[witness_idx];
    ChromaticWitness output = input;
    
    // Add new vertex with new color
    if (output.bag_size < 32) {
        output.coloring[output.bag_size] = new_color;
        output.bag_size++;
        output.num_colors = max(output.num_colors, new_color + 1);
        
        uint idx = atomic_fetch_add_explicit(&output_count, 1u, memory_order_relaxed);
        if (idx < 10000) {
            output_witnesses[idx] = output;
        }
    }
}

// GPU kernel for forget vertex operation  
kernel void forget_vertex_operation(
    device const ChromaticWitness* input_witnesses [[buffer(0)]],
    device const uint& input_count [[buffer(1)]],
    device const uint& forget_position [[buffer(2)]],
    device ChromaticWitness* output_witnesses [[buffer(3)]],
    device uint& output_count [[buffer(4)]],
    uint thread_id [[thread_position_in_grid]]
) {
    if (thread_id >= input_count) return;
    
    ChromaticWitness input = input_witnesses[thread_id];
    ChromaticWitness output;
    
    // Remove vertex at forget_position
    output.bag_size = input.bag_size - 1;
    output.valid = input.valid;
    output.num_colors = 0;
    
    uint out_pos = 0;
    for (uint i = 0; i < input.bag_size; i++) {
        if (i != forget_position) {
            output.coloring[out_pos] = input.coloring[i];
            output.num_colors = max(output.num_colors, input.coloring[i] + 1);
            out_pos++;
        }
    }
    
    uint idx = atomic_fetch_add_explicit(&output_count, 1u, memory_order_relaxed);
    if (idx < 10000) {
        output_witnesses[idx] = output;
    }
}