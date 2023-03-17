#pragma once

#include <dlfcn.h>

#include <iostream>
#include <memory>
#include <stdexcept>

#include "ct_string.hpp"

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
	std::string so_path;
	std::unique_ptr<void, AsFunctionObject<dlclose>> handle;
	std::tuple<typename NamedFunctionSignatures::ptr...> function_pointers;

	[[noreturn]] void fail() {
		if (auto error = dlerror()) {
			throw std::runtime_error("Failed loading shared object " + so_path +
									 ": " + error);
		}
		throw std::runtime_error("Failed loading " + so_path +
								 ". Failed finding more error information.");
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
		: so_path(so_path_), handle(dlopen(so_path.c_str(), RTLD_NOW)) {
		if (!handle) fail();
		auto load_function = [&](auto signature) {
			auto fptr = dlsym(handle.get(), signature.name().c_str());
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

