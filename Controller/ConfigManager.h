#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H

#include <string>
#include <map>
#include <variant>
#include <fstream>
#include <memory>

namespace TreeWidzard {

// Configuration value type
using ConfigValue = std::variant<std::string, int, double, bool>;

class ConfigManager {
private:
    std::map<std::string, ConfigValue> config_map;
    std::string config_file_path;
    
    static std::unique_ptr<ConfigManager> instance;

public:
    // Singleton pattern
    static ConfigManager& getInstance();
    
    // Load configuration from file
    bool loadFromFile(const std::string& filepath);
    
    // Save configuration to file
    bool saveToFile(const std::string& filepath = "");
    
    // Set configuration values
    void set(const std::string& key, const ConfigValue& value);
    
    // Get configuration values with type safety
    template<typename T>
    T get(const std::string& key, const T& default_value = T{}) const;
    
    // Check if key exists
    bool exists(const std::string& key) const;
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    
    // Load default configuration
    void loadDefaults();
    
    // Validate configuration
    bool validate() const;
    
    // Print current configuration
    void printConfig() const;

private:
    ConfigManager() = default;
    
    // Helper methods
    std::string valueToString(const ConfigValue& value) const;
    ConfigValue stringToValue(const std::string& str, const std::string& type) const;
    bool parseLine(const std::string& line);
};

// Template implementation
template<typename T>
T ConfigManager::get(const std::string& key, const T& default_value) const {
    auto it = config_map.find(key);
    if (it == config_map.end()) {
        return default_value;
    }
    
    try {
        return std::get<T>(it->second);
    } catch (const std::bad_variant_access&) {
        return default_value;
    }
}

// Configuration sections for better organization
namespace Config {
    namespace System {
        constexpr const char* DEFAULT_THREAD_COUNT = "system.default_thread_count";
        constexpr const char* MAX_MEMORY_MB = "system.max_memory_mb";
        constexpr const char* TEMP_DIRECTORY = "system.temp_directory";
        constexpr const char* LOG_LEVEL = "system.log_level";
    }
    
    namespace Search {
        constexpr const char* DEFAULT_STRATEGY = "search.default_strategy";
        constexpr const char* TIMEOUT_SECONDS = "search.timeout_seconds";
        constexpr const char* USE_ISOMORPHISM_REDUCTION = "search.use_isomorphism_reduction";
        constexpr const char* PARALLEL_ENABLED = "search.parallel_enabled";
    }
    
    namespace DP {
        constexpr const char* DEFAULT_WIDTH_TYPE = "dp.default_width_type";
        constexpr const char* MAX_WIDTH = "dp.max_width";
        constexpr const char* CORE_DIRECTORIES = "dp.core_directories";
        constexpr const char* CACHE_ENABLED = "dp.cache_enabled";
    }
    
    namespace Output {
        constexpr const char* DEFAULT_FORMAT = "output.default_format";
        constexpr const char* VERBOSE_MODE = "output.verbose_mode";
        constexpr const char* SAVE_WITNESSES = "output.save_witnesses";
        constexpr const char* OUTPUT_DIRECTORY = "output.output_directory";
    }
    
    namespace Debug {
        constexpr const char* ENABLE_PROFILING = "debug.enable_profiling";
        constexpr const char* SAVE_INTERMEDIATE_RESULTS = "debug.save_intermediate_results";
        constexpr const char* VALIDATE_INPUTS = "debug.validate_inputs";
    }
}

// Configuration file format helper
class ConfigFileFormat {
public:
    static const std::string DEFAULT_CONFIG_CONTENT;
    static void createDefaultConfigFile(const std::string& filepath);
    static bool isValidConfigFile(const std::string& filepath);
};

} // namespace TreeWidzard

#endif // CONFIG_MANAGER_H