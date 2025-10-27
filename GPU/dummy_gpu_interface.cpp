// Dummy GPU interface for non-Apple platforms
extern "C" {
    bool initialize_apple_silicon_gpu() {
        return false;
    }
    
    void get_gpu_info(char* name, size_t* memory) {
        strcpy(name, "CPU Only");
        *memory = 0;
    }
    
    void cleanup_apple_silicon_gpu() {
        // Nothing to do
    }
}