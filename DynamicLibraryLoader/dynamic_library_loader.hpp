#pragma once

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#else
#include <dlfcn.h>
#endif

#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <type_traits>

#include "ct_string.hpp"

#ifndef TREEWIDZARD_KEEP_DYNAMIC_LIBRARIES_LOADED
#if defined(__SANITIZE_ADDRESS__) || defined(__SANITIZE_THREAD__)
#define TREEWIDZARD_KEEP_DYNAMIC_LIBRARIES_LOADED 1
#elif defined(__has_feature)
#if __has_feature(address_sanitizer) || __has_feature(thread_sanitizer)
#define TREEWIDZARD_KEEP_DYNAMIC_LIBRARIES_LOADED 1
#endif
#endif
#endif

#ifndef TREEWIDZARD_KEEP_DYNAMIC_LIBRARIES_LOADED
#define TREEWIDZARD_KEEP_DYNAMIC_LIBRARIES_LOADED 0
#endif

template <CTString name, class Signature>
struct NamedFunctionSignature {};

template <CTString name_, class ReturnType, class... ArgumentTypes>
struct NamedFunctionSignature<name_, ReturnType(ArgumentTypes...)> {
	static constexpr auto name() -> CTString<name_.size() + 1> { return name_; }
	using ptr = ReturnType (*)(ArgumentTypes...);
};

template <auto F>
struct AsFunctionObject {
	decltype(auto) operator()(auto &&...args) const {
		return F(std::forward<decltype(args)>(args)...);
	}
};

template <class... NamedFunctionSignatures>
class DynamicLibraryLoader {
#ifdef _WIN32
	using native_handle = HMODULE;
#else
	using native_handle = void *;
#endif

	struct DynamicLibraryCloser {
		void operator()(std::remove_pointer_t<native_handle> *handle) const {
			if (!handle) return;
#if TREEWIDZARD_KEEP_DYNAMIC_LIBRARIES_LOADED
			// Sanitizer runtimes do not reliably support unloading and reloading
			// instrumented plugins in one process.
			(void)handle;
#else
#ifdef _WIN32
			FreeLibrary(handle);
#else
			dlclose(handle);
#endif
#endif
		}
	};

	static auto open_library(const char *path) -> native_handle {
#ifdef _WIN32
		return LoadLibraryA(path);
#else
		return dlopen(path, RTLD_NOW);
#endif
	}

	static auto lookup_symbol(native_handle handle, const char *name) -> void * {
#ifdef _WIN32
		return reinterpret_cast<void *>(GetProcAddress(handle, name));
#else
		return dlsym(handle, name);
#endif
	}

	static auto last_error_string() -> std::string {
#ifdef _WIN32
		const DWORD error = GetLastError();
		if (error == 0) {
			return "unknown Windows loader error";
		}

		LPSTR buffer = nullptr;
		const DWORD flags = FORMAT_MESSAGE_ALLOCATE_BUFFER |
							FORMAT_MESSAGE_FROM_SYSTEM |
							FORMAT_MESSAGE_IGNORE_INSERTS;
		const DWORD size = FormatMessageA(
			flags, nullptr, error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			reinterpret_cast<LPSTR>(&buffer), 0, nullptr);
		std::string message =
			size != 0 ? std::string(buffer, size)
					  : "Windows loader error code " + std::to_string(error);
		if (buffer) LocalFree(buffer);
		return message;
#else
		if (auto error = dlerror()) {
			return error;
		}
		return "failed finding more error information";
#endif
	}

	std::string so_path;
	std::unique_ptr<std::remove_pointer_t<native_handle>, DynamicLibraryCloser>
		handle;
	std::tuple<typename NamedFunctionSignatures::ptr...> function_pointers;

	[[noreturn]] void fail() {
		throw std::runtime_error("Failed loading shared object " + so_path +
								 ": " + last_error_string());
	}

	template <CTString name, std::size_t index, class Head, class... Tail>
	decltype(auto) call_if_matching_name(auto &&...args) const {
		if constexpr (name == Head::name())
			return std::get<index>(function_pointers)(
				std::forward<decltype(args)>(args)...);
		else
			return call_if_matching_name<name, index + 1, Tail...>(
				std::forward<decltype(args)>(args)...);
	}

  public:
	DynamicLibraryLoader(const std::string &so_path_)
		: so_path(so_path_), handle(open_library(so_path.c_str())) {
		if (!handle) fail();
		auto load_function = [&](auto signature) {
			auto fptr = lookup_symbol(handle.get(), signature.name().c_str());
			if (!fptr) fail();
			return reinterpret_cast<typename decltype(signature)::ptr>(fptr);
		};
		function_pointers =
			std::make_tuple(load_function(NamedFunctionSignatures{})...);

		/*
		std::cerr << "\033[32;1mLoaded " << so_path << " into " << this
				  << "\033[m\n";
		// */
	}
	DynamicLibraryLoader(DynamicLibraryLoader &&other)
		: so_path(std::move(other.so_path)),
		  handle(std::move(other.handle)),
		  function_pointers(std::move(other.function_pointers)) {
		/*
		std::cerr << "\033[33;1mMoved " << so_path << " from " << &other
				  << " to " << this << "\033[m\n";
		// */
	}
	DynamicLibraryLoader &operator=(DynamicLibraryLoader &&other) {
		/*
		if (handle)
			std::cerr << "\031[31;1mUnloaded " << so_path << " from " << this
					  << " (in move assignment) \033[m\n";
		// */
		so_path = std::move(other.so_path);
		handle = std::move(other.handle);
		function_pointers = std::move(other.function_pointers);
		/*
		std::cerr << "\033[33;1mMoved " << so_path << " from " << &other
				  << " to " << this << "\033[m\n";
		// */
		return *this;
	}

	DynamicLibraryLoader(const DynamicLibraryLoader &) = delete;
	DynamicLibraryLoader &operator=(const DynamicLibraryLoader &) = delete;

	~DynamicLibraryLoader() {
		/*
		if (handle)
			std::cerr << "\033[31;1mUnloaded " << so_path << " from " << this
					  << "\033[m\n";
		// */
	}

	template <CTString name>
	decltype(auto) call(auto &&...args) const {
		if (!handle)
			throw std::runtime_error(
				"Tried to call function in moved-from DynamicLibraryLoader.");
		return call_if_matching_name<name, 0, NamedFunctionSignatures...>(
			std::forward<decltype(args)>(args)...);
	}
};
