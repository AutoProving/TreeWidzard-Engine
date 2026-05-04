// SPDX-License-Identifier: MIT
// Copyright (c) 2020-2026 Sam Urmian, Mateus de Oliveira Oliveira, and contributors.

#ifndef ERROR_HANDLING_H
#define ERROR_HANDLING_H

#include <string>
#include <exception>
#include <memory>

namespace TreeWidzard {

// Custom exception hierarchy for better error handling
class TreeWidzardException : public std::exception {
private:
    std::string message;
    int error_code;

public:
    TreeWidzardException(const std::string& msg, int code = 0) 
        : message(msg), error_code(code) {}
    
    const char* what() const noexcept override {
        return message.c_str();
    }
    
    int getErrorCode() const { return error_code; }
};

class ParseException : public TreeWidzardException {
public:
    ParseException(const std::string& msg, int line = -1, int column = -1)
        : TreeWidzardException(formatMessage(msg, line, column), 1) {}

private:
    static std::string formatMessage(const std::string& msg, int line, int column) {
        std::string result = "Parse Error: " + msg;
        if (line >= 0) {
            result += " at line " + std::to_string(line);
            if (column >= 0) {
                result += ", column " + std::to_string(column);
            }
        }
        return result;
    }
};

class CoreException : public TreeWidzardException {
public:
    CoreException(const std::string& core_name, const std::string& msg)
        : TreeWidzardException("Core '" + core_name + "': " + msg, 2) {}
};

class FileException : public TreeWidzardException {
public:
    FileException(const std::string& filename, const std::string& msg)
        : TreeWidzardException("File '" + filename + "': " + msg, 3) {}
};

class ValidationException : public TreeWidzardException {
public:
    ValidationException(const std::string& msg)
        : TreeWidzardException("Validation Error: " + msg, 4) {}
};

// Error handling utilities
class ErrorHandler {
public:
    static void handleError(const TreeWidzardException& e, bool exit_on_error = true);
    static void logWarning(const std::string& message);
    static void logInfo(const std::string& message);
    static void setVerbose(bool verbose) { is_verbose = verbose; }

private:
    static bool is_verbose;
};

// RAII wrapper for better resource management
template<typename T>
class ResourceGuard {
private:
    std::unique_ptr<T> resource;
    std::function<void(T*)> cleanup;

public:
    ResourceGuard(T* ptr, std::function<void(T*)> cleanup_func)
        : resource(ptr), cleanup(cleanup_func) {}
    
    ~ResourceGuard() {
        if (resource && cleanup) {
            cleanup(resource.get());
        }
    }
    
    T* get() { return resource.get(); }
    T* release() { return resource.release(); }
};

// Input validation utilities
class Validator {
public:
    static void validateTreeDecomposition(const std::string& filename);
    static void validateConjecture(const std::string& conjecture_text);
    static void validateGraphFile(const std::string& filename);
    static void validateWidthBounds(int width, const std::string& type);
};

} // namespace TreeWidzard

// Convenient macros for error handling
#define TW_THROW_PARSE(msg, line, col) \
    throw TreeWidzard::ParseException(msg, line, col)

#define TW_THROW_CORE(core, msg) \
    throw TreeWidzard::CoreException(core, msg)

#define TW_THROW_FILE(file, msg) \
    throw TreeWidzard::FileException(file, msg)

#define TW_THROW_VALIDATION(msg) \
    throw TreeWidzard::ValidationException(msg)

#define TW_HANDLE_ERROR(e) \
    TreeWidzard::ErrorHandler::handleError(e, false)

#endif // ERROR_HANDLING_H