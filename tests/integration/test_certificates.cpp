#include "../utils/test_helpers.h"
#include "../utils/cli_test_support.h"

#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include <gtest/gtest.h>

namespace fs = std::filesystem;
using TreeWidzard::Testing::find_repo_root;
using TreeWidzard::Testing::launcher_command;
using TreeWidzard::Testing::launcher_path;
using TreeWidzard::Testing::run_cmd;
using TreeWidzard::Testing::shell_quote;

namespace {

struct TempDir {
	fs::path path;
	~TempDir() {
		std::error_code ec;
		fs::remove_all(path, ec);
	}
};

std::vector<std::string> read_lines(const fs::path &p) {
	std::ifstream in(p);
	if (!in) throw std::runtime_error("failed to read: " + p.string());
	std::vector<std::string> lines;
	std::string line;
	while (std::getline(in, line)) lines.push_back(line);
	return lines;
}

void write_lines(const fs::path &p, const std::vector<std::string> &lines) {
	std::ofstream out(p, std::ios::out | std::ios::trunc);
	if (!out) throw std::runtime_error("failed to write: " + p.string());
	for (const auto &l : lines) out << l << "\n";
}

TempDir make_temp_dir() {
	const auto now = std::chrono::high_resolution_clock::now().time_since_epoch();
	const auto stamp =
		std::chrono::duration_cast<std::chrono::nanoseconds>(now).count();
	fs::path dir = fs::path("tests/data") / ("temp_cert_" + std::to_string(stamp));
	fs::create_directories(dir);
	return TempDir{dir};
}

} // namespace

TEST(Certificates, BFS_SAT_GeneratesAndChecks) {
	const fs::path root = find_repo_root();
	const auto tmp = make_temp_dir();

	const fs::path property = root / "examples/conjectures/simple_implies.txt";
	const fs::path cert = tmp.path / "bfs_sat.twzcert";

	const std::string gen =
		launcher_command(root) +
		" -atp tw=2 BreadthFirstSearch " + shell_quote(property.string()) +
		" -cert " + shell_quote(cert.string());
	ASSERT_EQ(run_cmd(gen), 0);
	ASSERT_TRUE(fs::exists(cert));

	const std::string chk =
		launcher_command(root) + " -checkcert " +
		shell_quote(property.string()) + " " + shell_quote(cert.string());
	ASSERT_EQ(run_cmd(chk), 0);
}

TEST(Certificates, ISO_SAT_GeneratesAndChecks) {
	const fs::path root = find_repo_root();
	const auto tmp = make_temp_dir();

	const fs::path property = root / "examples/conjectures/simple_implies.txt";
	const fs::path cert = tmp.path / "iso_sat.twzcert";

	const std::string gen =
		launcher_command(root) +
		" -atp tw=2 IsomorphismBreadthFirstSearch " +
		shell_quote(property.string()) + " -cert " + shell_quote(cert.string());
	ASSERT_EQ(run_cmd(gen), 0);
	ASSERT_TRUE(fs::exists(cert));

	const std::string chk =
		launcher_command(root) + " -checkcert " +
		shell_quote(property.string()) + " " + shell_quote(cert.string());
	ASSERT_EQ(run_cmd(chk), 0);
}

TEST(Certificates, PBFS_SAT_GeneratesAndChecks) {
	const fs::path root = find_repo_root();
	const auto tmp = make_temp_dir();

	const fs::path property = root / "examples/conjectures/simple_implies.txt";
	const fs::path cert = tmp.path / "pbfs_sat.twzcert";

	const std::string gen =
		launcher_command(root) +
		" -atp tw=2 -nthreads 2 ParallelBreadthFirstSearch " +
		shell_quote(property.string()) + " -cert " + shell_quote(cert.string());
	ASSERT_EQ(run_cmd(gen), 0);
	ASSERT_TRUE(fs::exists(cert));

	const std::string chk =
		launcher_command(root) + " -checkcert " +
		shell_quote(property.string()) + " " + shell_quote(cert.string());
	ASSERT_EQ(run_cmd(chk), 0);
}

TEST(Certificates, PISO_SAT_GeneratesAndChecks) {
	const fs::path root = find_repo_root();
	const auto tmp = make_temp_dir();

	const fs::path property = root / "examples/conjectures/simple_implies.txt";
	const fs::path cert = tmp.path / "piso_sat.twzcert";

	const std::string gen =
		launcher_command(root) +
		" -atp tw=2 -nthreads 2 ParallelIsomorphismBreadthFirstSearch " +
		shell_quote(property.string()) + " -cert " + shell_quote(cert.string());
	ASSERT_EQ(run_cmd(gen), 0);
	ASSERT_TRUE(fs::exists(cert));

	const std::string chk =
		launcher_command(root) + " -checkcert " +
		shell_quote(property.string()) + " " + shell_quote(cert.string());
	ASSERT_EQ(run_cmd(chk), 0);
}

TEST(Certificates, Premise_BFS_SAT_GeneratesAndChecks) {
	const fs::path root = find_repo_root();
	const auto tmp = make_temp_dir();

	const fs::path property = root / "examples/conjectures/simple_implies.txt";
	const fs::path cert = tmp.path / "bfs_premise_sat.twzcert";

	const std::string gen =
		launcher_command(root) +
		" -atp tw=2 -premise BreadthFirstSearch " +
		shell_quote(property.string()) + " -cert " + shell_quote(cert.string());
	ASSERT_EQ(run_cmd(gen), 0);
	ASSERT_TRUE(fs::exists(cert));

	const std::string chk =
		launcher_command(root) + " -checkcert " +
		shell_quote(property.string()) + " " + shell_quote(cert.string());
	ASSERT_EQ(run_cmd(chk), 0);

	const auto lines = read_lines(cert);
	ASSERT_TRUE(std::any_of(lines.begin(), lines.end(), [](const std::string &l) {
		return l == "H PREMISE 1";
	}));
}

TEST(Certificates, BFS_NO_GeneratesAndChecks) {
	const fs::path root = find_repo_root();
	const auto tmp = make_temp_dir();

	const fs::path property = root / "examples/conjectures/simple_3_coloring.txt";
	const fs::path cert = tmp.path / "bfs_no.twzcert";

	const std::string gen =
		launcher_command(root) +
		" -atp tw=3 BreadthFirstSearch " + shell_quote(property.string()) +
		" -cert " + shell_quote(cert.string());
	ASSERT_EQ(run_cmd(gen), 0);
	ASSERT_TRUE(fs::exists(cert));

	const std::string chk =
		launcher_command(root) + " -checkcert " +
		shell_quote(property.string()) + " " + shell_quote(cert.string());
	ASSERT_EQ(run_cmd(chk), 0);
}

TEST(Certificates, Negative_TruncatedSATRejected) {
	const fs::path root = find_repo_root();
	const auto tmp = make_temp_dir();

	const fs::path property = root / "examples/conjectures/simple_implies.txt";
	const fs::path cert = tmp.path / "bfs_sat.twzcert";

	const std::string gen =
		launcher_command(root) +
		" -atp tw=2 BreadthFirstSearch " + shell_quote(property.string()) +
		" -cert " + shell_quote(cert.string());
	ASSERT_EQ(run_cmd(gen), 0);

	// Create a truncated certificate: header + leaf + SAT result.
	const fs::path truncated = tmp.path / "bfs_sat_truncated.twzcert";
	const auto lines = read_lines(cert);
	std::vector<std::string> out;
	for (const auto &l : lines) {
		out.push_back(l);
		if (l == "S 0 LEAF") break;
	}
	out.push_back("R SATISFIED");
	write_lines(truncated, out);

	const std::string chk =
		launcher_command(root) + " -checkcert " +
		shell_quote(property.string()) + " " + shell_quote(truncated.string());
	ASSERT_NE(run_cmd(chk), 0);
}

TEST(Certificates, Negative_WrongPropertyRejected) {
	const fs::path root = find_repo_root();
	const auto tmp = make_temp_dir();

	const fs::path property_ok = root / "examples/conjectures/simple_implies.txt";
	const fs::path property_bad =
		root / "examples/conjectures/simple_3_coloring.txt";
	const fs::path cert = tmp.path / "bfs_sat.twzcert";

	const std::string gen =
		launcher_command(root) +
		" -atp tw=2 BreadthFirstSearch " + shell_quote(property_ok.string()) +
		" -cert " + shell_quote(cert.string());
	ASSERT_EQ(run_cmd(gen), 0);

	const std::string chk =
		launcher_command(root) + " -checkcert " +
		shell_quote(property_bad.string()) + " " + shell_quote(cert.string());
	ASSERT_NE(run_cmd(chk), 0);
}

TEST(Certificates, Negative_TamperedHeaderRejected) {
	const fs::path root = find_repo_root();
	const auto tmp = make_temp_dir();

	const fs::path property = root / "examples/conjectures/simple_implies.txt";
	const fs::path cert = tmp.path / "bfs_sat.twzcert";

	const std::string gen =
		launcher_command(root) +
		" -atp tw=2 BreadthFirstSearch " + shell_quote(property.string()) +
		" -cert " + shell_quote(cert.string());
	ASSERT_EQ(run_cmd(gen), 0);

	const auto lines = read_lines(cert);
	std::vector<std::string> out = lines;
	for (auto &l : out) {
		if (l.rfind("H DPCORES_HASH ", 0) == 0) {
			l = "H DPCORES_HASH 0x0000000000000000";
			break;
		}
	}
	const fs::path tampered = tmp.path / "bfs_sat_badplugins.twzcert";
	write_lines(tampered, out);

	const std::string chk =
		launcher_command(root) + " -checkcert " +
		shell_quote(property.string()) + " " + shell_quote(tampered.string());
	ASSERT_NE(run_cmd(chk), 0);
}

TEST(Certificates, Negative_PremiseRequiresImplication) {
	const fs::path root = find_repo_root();
	const auto tmp = make_temp_dir();

	const fs::path property = root / "examples/conjectures/simple_3_coloring.txt";
	const fs::path cert = tmp.path / "bfs_no.twzcert";

	const std::string gen =
		launcher_command(root) +
		" -atp tw=3 BreadthFirstSearch " + shell_quote(property.string()) +
		" -cert " + shell_quote(cert.string());
	ASSERT_EQ(run_cmd(gen), 0);

	auto lines = read_lines(cert);
	for (auto &l : lines) {
		if (l == "H PREMISE 0") {
			l = "H PREMISE 1";
			break;
		}
	}
	const fs::path tampered = tmp.path / "bfs_no_badpremise.twzcert";
	write_lines(tampered, lines);

	const std::string chk =
		launcher_command(root) + " -checkcert " +
		shell_quote(property.string()) + " " + shell_quote(tampered.string());
	ASSERT_NE(run_cmd(chk), 0);
}

TEST(Certificates, Negative_InvalidOpRejected) {
	const fs::path root = find_repo_root();
	const auto tmp = make_temp_dir();

	const fs::path property = root / "examples/conjectures/simple_implies.txt";
	const fs::path cert = tmp.path / "bfs_sat.twzcert";

	const std::string gen =
		launcher_command(root) +
		" -atp tw=2 BreadthFirstSearch " + shell_quote(property.string()) +
		" -cert " + shell_quote(cert.string());
	ASSERT_EQ(run_cmd(gen), 0);

	auto lines = read_lines(cert);
	// Make the first INTRO_V introduce an out-of-range label for k=2 (labels are 1..3).
	for (auto &l : lines) {
		if (l.rfind("S ", 0) == 0 && l.find(" INTRO_V ") != std::string::npos) {
			std::istringstream is(l);
			std::string S, kind;
			int id = -1, parent = -1;
			unsigned u = 0;
			is >> S >> id >> kind >> parent >> u;
			l = "S " + std::to_string(id) + " INTRO_V " + std::to_string(parent) +
				" 4";
			break;
		}
	}
	const fs::path invalid = tmp.path / "bfs_sat_invalid.twzcert";
	write_lines(invalid, lines);

	const std::string chk =
		launcher_command(root) + " -checkcert " +
		shell_quote(property.string()) + " " + shell_quote(invalid.string());
	ASSERT_NE(run_cmd(chk), 0);
}

TEST(Certificates, Negative_InvalidJoinMapRejected) {
	const fs::path root = find_repo_root();
	const auto tmp = make_temp_dir();

	const fs::path property = root / "examples/conjectures/simple_implies.txt";
	const fs::path cert = tmp.path / "iso_sat.twzcert";

	const std::string gen =
		launcher_command(root) +
		" -atp tw=2 IsomorphismBreadthFirstSearch " +
		shell_quote(property.string()) + " -cert " + shell_quote(cert.string());
	ASSERT_EQ(run_cmd(gen), 0);

	auto lines = read_lines(cert);
	for (auto &l : lines) {
		if (l.rfind("S ", 0) == 0 && l.find(" JOIN ") != std::string::npos &&
			l.find(" MAP ") != std::string::npos) {
			// Break the map by changing it to a non-permutation.
			const auto pos = l.find(" MAP ");
			l = l.substr(0, pos) + " MAP 1 2";
			break;
		}
	}
	const fs::path invalid = tmp.path / "iso_sat_badmap.twzcert";
	write_lines(invalid, lines);

	const std::string chk =
		launcher_command(root) + " -checkcert " +
		shell_quote(property.string()) + " " + shell_quote(invalid.string());
	ASSERT_NE(run_cmd(chk), 0);
}
