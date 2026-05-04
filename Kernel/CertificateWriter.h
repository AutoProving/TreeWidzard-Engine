// SPDX-License-Identifier: MIT
// Copyright (c) 2020-2026 Sam Urmian, Mateus de Oliveira Oliveira, and contributors.

#ifndef TREEWIDZARD_CERTIFICATE_WRITER_H
#define TREEWIDZARD_CERTIFICATE_WRITER_H

#include <cstdint>
#include <fstream>
#include <map>
#include <optional>
#include <string>

#include "Width.h"

namespace TreeWidzard::Certificates {

enum class CanonMode {
	NONE,
	BAG_MIN,
};

class CertificateWriter {
  public:
	explicit CertificateWriter(const std::string &path);

	void writeHeader(const Width &width, const std::string &search_name,
					 CanonMode canon_mode, bool premise_enabled,
					 uint64_t property_hash, const std::string &property_path,
					 std::optional<uint64_t> dpcores_hash);

	void writeLeaf(int id);
	void writeIntroVertex(int id, int parent, unsigned u);
	void writeForgetVertex(int id, int parent, unsigned u);
	void writeIntroEdge(int id, int parent, unsigned u, unsigned v);
	void writeJoin(int id, int left, int right,
				   const std::optional<std::map<unsigned, unsigned>> &map);

	void writeResultSatisfied();
	void writeResultNotSatisfied(int bad_id);
	void writeResultIncomplete();

  private:
	std::ofstream out_;
};

} // namespace TreeWidzard::Certificates

#endif
