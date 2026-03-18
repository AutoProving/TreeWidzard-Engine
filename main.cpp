#include <chrono>
#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>

#include <sys/time.h>
#include <unistd.h>

#include "Controller/CertificateChecker.h"
#include "Controller/Parser/command_parser.hpp"

extern std::FILE *command_in;

namespace {

auto set_environment_variable(const char *name, const std::string &value)
    -> int {
#ifdef _WIN32
	return _putenv_s(name, value.c_str());
#else
	return setenv(name, value.c_str(), 1);
#endif
}

} // namespace

int main(int argc, char *arg[]) {
	// Pre-parse argv for certificate options without touching the generated
	// command parser.
	std::vector<std::string> filtered_args;
	filtered_args.reserve(static_cast<size_t>(argc > 0 ? argc - 1 : 0));

	for (int i = 1; i < argc; ++i) {
		const std::string current = arg[i];
		if (current == "-cert" || current == "--cert") {
			if (i + 1 >= argc) {
				std::cerr << "Error: usage: -cert <file>" << std::endl;
				return 20;
			}
			const std::string path = arg[i + 1];
			if (set_environment_variable("TREEWIDZARD_CERT_PATH", path) != 0) {
				std::cerr << "Error: failed to set TREEWIDZARD_CERT_PATH."
					  << std::endl;
				return 20;
			}
			i++;
			continue;
		}
		if (current == "-checkcert" || current == "--checkcert") {
			if (i + 2 >= argc) {
				std::cerr
					<< "Error: usage: -checkcert <property_file> <certificate_file>"
					<< std::endl;
				return 20;
			}
			const std::string property_file = arg[i + 1];
			const std::string certificate_file = arg[i + 2];
			const bool ok =
				check_certificate_file(property_file, certificate_file);
			return ok ? 0 : 20;
		}
		filtered_args.push_back(current);
	}

	// Reading the input and put it in string _all_arg
	std::string _all_arg;
	for (size_t idx = 0; idx < filtered_args.size(); idx++) {
		std::string current_arg = filtered_args[idx];
		
		// User-friendly fix: normalize spacing around equals sign
		// Convert "tw=3" to "tw = 3" and "pw=4" to "pw = 4"
		if ((current_arg == "tw" || current_arg == "pw") &&
			idx + 1 < filtered_args.size()) {
			std::string next_arg = filtered_args[idx + 1];
			if (next_arg.length() > 1 && next_arg[0] == '=' && next_arg[1] != '=') {
				// Handle "tw=3" -> "tw = 3"
				_all_arg += current_arg + "\n" + "=" + "\n" + next_arg.substr(1);
				idx++; // Skip next argument since we processed it
			} else {
				_all_arg += current_arg;
			}
		} else {
			// Check if current argument is like "tw=3" or "pw=4"
			size_t eq_pos = current_arg.find('=');
			if (eq_pos != std::string::npos && eq_pos > 0 && eq_pos < current_arg.length() - 1) {
				if (current_arg.substr(0, eq_pos) == "tw" || current_arg.substr(0, eq_pos) == "pw") {
					// Split "tw=3" into "tw = 3"
					_all_arg += current_arg.substr(0, eq_pos) + "\n" + "=" + "\n" + current_arg.substr(eq_pos + 1);
				} else {
					_all_arg += current_arg;
				}
			} else {
				_all_arg += current_arg;
			}
		}
		
		if (idx != filtered_args.size() - 1) {
			_all_arg += "\n";
		} else {
			_all_arg += "\n;";
		}
	}

	// Generating the temp file pFile
	std::FILE *pFile;
	pFile = tmpfile(); // c++ function for generating a temporary file
	if (!pFile) {
		std::cerr << "Error: failed to create a temporary command file." << std::endl;
		return 20;
	}

	std::fputs(_all_arg.c_str(), pFile); // Write _all_arg in pFile
	rewind(pFile); // Sets the position indicator associated with stream to the
				   // beginning of the file.
	command_in = pFile; // Set command_in equals to pFile

	/* Test to print the content of command_in
	char buffer [256];
	while (!feof(command_in)) {
		if (fgets (buffer,256,command_in) == NULL) break;
		fputs (buffer,stdout);
	}
	rewind(pFile);
	*/

	/* Test for checking the directory of the temporary file
		int MAXSIZE = 0xFFF;
		char proclnk[0xFFF];
		char filename[0xFFF];
		FILE *fp;
		int fno;
		ssize_t r;
		fp = pFile;

		if (fp != NULL)
		{
			fno = fileno(fp);
			sprintf(proclnk, "/proc/self/fd/%d", fno);
			r = readlink(proclnk, filename, MAXSIZE);
			if (r < 0)
			{
				printf("failed to readlink\n");
				exit(1);
			}
			filename[r] = '\0';
			printf("fp -> fno -> filename: %p -> %d -> %s\n",
				   fp, fno, filename);
		}
	*/

	// Calling the input parser
	int result_arg = 10;
	std::string width_type;
	int width_value;
	const int parse_status = command_parse(result_arg, width_type, width_value);

	fclose(command_in); // closing command_in

	return parse_status == 0 ? 0 : 20;
}
