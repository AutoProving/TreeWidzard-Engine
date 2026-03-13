#ifndef TREEWIDZARD_CERTIFICATE_UTILS_H
#define TREEWIDZARD_CERTIFICATE_UTILS_H

#include <algorithm>
#include <array>
#include <cstdint>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <optional>
#include <sstream>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "PathList.h"

#ifndef TREEWIDZARD_DPCORES_DEFAULT
#define TREEWIDZARD_DPCORES_DEFAULT ""
#endif

#ifdef _WIN32
	#define TREEWIDZARD_DYNAMIC_LIB_EXTENSION ".dll"
#elif __APPLE__
	#define TREEWIDZARD_DYNAMIC_LIB_EXTENSION ".dylib"
#elif __linux__
	#define TREEWIDZARD_DYNAMIC_LIB_EXTENSION ".so"
#else
	#error "Unsupported operating system"
#endif

namespace TreeWidzard::Certificates {

inline uint64_t fnv1a64_update(uint64_t hash, const uint8_t *data, size_t n) {
	// 64-bit FNV-1a.
	const uint64_t prime = 1099511628211ULL;
	for (size_t i = 0; i < n; ++i) {
		hash ^= static_cast<uint64_t>(data[i]);
		hash *= prime;
	}
	return hash;
}

inline uint64_t fnv1a64_bytes(std::string_view s) {
	const uint64_t offset_basis = 14695981039346656037ULL;
	return fnv1a64_update(offset_basis,
						  reinterpret_cast<const uint8_t *>(s.data()), s.size());
}

inline uint64_t fnv1a64_file(const std::filesystem::path &path) {
	const uint64_t offset_basis = 14695981039346656037ULL;
	std::ifstream in(path, std::ios::binary);
	if (!in) {
		std::cerr << "Error: failed to open file for hashing: " << path
				  << std::endl;
		exit(20);
	}

	std::array<char, 1 << 16> buf{};
	uint64_t h = offset_basis;
	while (in) {
		in.read(buf.data(), static_cast<std::streamsize>(buf.size()));
		const auto got = in.gcount();
		if (got <= 0) break;
		h = fnv1a64_update(h, reinterpret_cast<const uint8_t *>(buf.data()),
						   static_cast<size_t>(got));
	}
	return h;
}

inline std::string hex_u64(uint64_t x) {
	std::ostringstream os;
	os << "0x" << std::hex << std::setw(16) << std::setfill('0') << x;
	return os.str();
}

inline std::optional<uint64_t> parse_hex_u64(std::string_view s) {
	if (s.size() < 3) return std::nullopt;
	if (s[0] != '0' || (s[1] != 'x' && s[1] != 'X')) return std::nullopt;
	uint64_t value = 0;
	for (size_t i = 2; i < s.size(); ++i) {
		const char c = s[i];
		uint8_t nibble = 0;
		if (c >= '0' && c <= '9') {
			nibble = static_cast<uint8_t>(c - '0');
		} else if (c >= 'a' && c <= 'f') {
			nibble = static_cast<uint8_t>(10 + (c - 'a'));
		} else if (c >= 'A' && c <= 'F') {
			nibble = static_cast<uint8_t>(10 + (c - 'A'));
		} else {
			return std::nullopt;
		}
		value = (value << 4) | static_cast<uint64_t>(nibble);
	}
	return value;
}

inline std::vector<std::string> split_paths(const std::string &paths) {
	return split_path_list(paths);
}

struct DPCoreFileEntry {
	std::string name;
	uint64_t file_hash;
};

inline std::vector<std::string> default_dp_core_paths() {
	if (const char *paths = std::getenv("TREEWIDZARD_DPCORES")) {
		return split_paths(paths);
	}
	return split_paths(TREEWIDZARD_DPCORES_DEFAULT);
}

inline std::vector<DPCoreFileEntry> list_dp_core_files() {
	std::vector<DPCoreFileEntry> entries;
	for (const auto &dir : default_dp_core_paths()) {
		if (dir.empty()) continue;
		if (!std::filesystem::exists(dir)) continue;
		for (const auto &entry : std::filesystem::directory_iterator(dir)) {
			const std::string path = entry.path().string();
			if (path.find(TREEWIDZARD_DYNAMIC_LIB_EXTENSION) ==
				std::string::npos) {
				continue;
			}
			const std::filesystem::path p(path);
			entries.push_back(
				DPCoreFileEntry{p.filename().string(), fnv1a64_file(p)});
		}
	}
	std::sort(entries.begin(), entries.end(),
			  [](const DPCoreFileEntry &a, const DPCoreFileEntry &b) {
				  return a.name < b.name;
			  });
	return entries;
}

inline uint64_t dp_cores_fingerprint() {
	const uint64_t offset_basis = 14695981039346656037ULL;
	uint64_t h = offset_basis;
	for (const auto &e : list_dp_core_files()) {
		h = fnv1a64_update(h, reinterpret_cast<const uint8_t *>(e.name.data()),
						   e.name.size());
		const std::array<uint8_t, 8> bytes = {
			static_cast<uint8_t>(e.file_hash & 0xffU),
			static_cast<uint8_t>((e.file_hash >> 8) & 0xffU),
			static_cast<uint8_t>((e.file_hash >> 16) & 0xffU),
			static_cast<uint8_t>((e.file_hash >> 24) & 0xffU),
			static_cast<uint8_t>((e.file_hash >> 32) & 0xffU),
			static_cast<uint8_t>((e.file_hash >> 40) & 0xffU),
			static_cast<uint8_t>((e.file_hash >> 48) & 0xffU),
			static_cast<uint8_t>((e.file_hash >> 56) & 0xffU),
		};
		h = fnv1a64_update(h, bytes.data(), bytes.size());
	}
	return h;
}

} // namespace TreeWidzard::Certificates

#endif
