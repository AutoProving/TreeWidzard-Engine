#import <Metal/Metal.h>
#import <MetalPerformanceShaders/MetalPerformanceShaders.h>
#import <Foundation/Foundation.h>

static id<MTLDevice> g_device = nil;
static id<MTLCommandQueue> g_command_queue = nil;

extern "C" {
    bool initialize_apple_silicon_gpu() {
        @autoreleasepool {
            g_device = MTLCreateSystemDefaultDevice();
            if (g_device == nil) {
                return false;
            }
            
            g_command_queue = [g_device newCommandQueue];
            if (g_command_queue == nil) {
                g_device = nil;
                return false;
            }
            
            return true;
        }
    }
    
    void get_gpu_info(char* name, size_t* memory) {
        @autoreleasepool {
            if (g_device != nil) {
                NSString* deviceName = [g_device name];
                const char* cName = [deviceName UTF8String];
                strcpy(name, cName);
                *memory = [g_device recommendedMaxWorkingSetSize];
            } else {
                strcpy(name, "No GPU Available");
                *memory = 0;
            }
        }
    }
    
    void cleanup_apple_silicon_gpu() {
        @autoreleasepool {
            if (g_command_queue != nil) {
                g_command_queue = nil;
            }
            if (g_device != nil) {
                g_device = nil;
            }
        }
    }
}