// SPDX-License-Identifier: MIT
// Copyright (c) 2020-2026 Sam Urmian, Mateus de Oliveira Oliveira, and contributors.

#include "CertificateWriter.h"

#include <iostream>

#include "CertificateUtils.h"

namespace TreeWidzard::Certificates {

CertificateWriter::CertificateWriter(const std::string &path)
	: out_(path, std::ios::out | std::ios::trunc) {
	if (!out_) {
		std::cerr << "Error: could not open certificate file for writing: "
				  << path << std::endl;
		exit(20);
	}
}

void CertificateWriter::writeHeader(const Width &width,
									const std::string &search_name,
									CanonMode canon_mode, bool premise_enabled,
									uint64_t property_hash,
									const std::string &property_path,
									std::optional<uint64_t> dpcores_hash) {
	out_ << "TWZCERT 1\n";
	out_ << "H WIDTH " << width.get_name() << " " << width.get_value() << "\n";
	out_ << "H SEARCH " << search_name << "\n";
	out_ << "H CANON "
		 << (canon_mode == CanonMode::NONE ? "NONE" : "BAG_MIN") << "\n";
	out_ << "H PREMISE " << (premise_enabled ? "1" : "0") << "\n";
	out_ << "H PROP_HASH " << hex_u64(property_hash) << "\n";
	if (!property_path.empty()) out_ << "H PROP_PATH " << property_path << "\n";
	if (dpcores_hash.has_value()) {
		out_ << "H DPCORES_HASH " << hex_u64(*dpcores_hash) << "\n";
	}
	out_.flush();
}

void CertificateWriter::writeLeaf(int id) {
	out_ << "S " << id << " LEAF\n";
	out_.flush();
}

void CertificateWriter::writeIntroVertex(int id, int parent, unsigned u) {
	out_ << "S " << id << " INTRO_V " << parent << " " << u << "\n";
	out_.flush();
}

void CertificateWriter::writeForgetVertex(int id, int parent, unsigned u) {
	out_ << "S " << id << " FORGET_V " << parent << " " << u << "\n";
	out_.flush();
}

void CertificateWriter::writeIntroEdge(int id, int parent, unsigned u,
									   unsigned v) {
	out_ << "S " << id << " INTRO_E " << parent << " " << u << " " << v << "\n";
	out_.flush();
}

void CertificateWriter::writeJoin(
	int id, int left, int right,
	const std::optional<std::map<unsigned, unsigned>> &map) {
	out_ << "S " << id << " JOIN " << left << " " << right;
	if (map.has_value()) {
		out_ << " MAP";
		for (const auto &kv : *map) out_ << " " << kv.first << " " << kv.second;
	}
	out_ << "\n";
	out_.flush();
}

void CertificateWriter::writeResultSatisfied() {
	out_ << "R SATISFIED\n";
	out_.flush();
}

void CertificateWriter::writeResultNotSatisfied(int bad_id) {
	out_ << "R NOT_SATISFIED " << bad_id << "\n";
	out_.flush();
}

void CertificateWriter::writeResultIncomplete() {
	out_ << "R INCOMPLETE\n";
	out_.flush();
}

} // namespace TreeWidzard::Certificates

