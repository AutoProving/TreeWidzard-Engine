#include <chrono>
#include <cstdio>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <iostream>

#include <sys/time.h>
#include <unistd.h>

#include "Controller/Parser/command_parser.hpp"

extern std::FILE *command_in;

int main(int argc, char *arg[]) {
	// Reading the input and put it in string _all_arg
	std::string _all_arg;
	for (int i = 1; i < argc; i++) {
		std::string current_arg = arg[i];
		
		// User-friendly fix: normalize spacing around equals sign
		// Convert "tw=3" to "tw = 3" and "pw=4" to "pw = 4"
		if ((current_arg == "tw" || current_arg == "pw") && i + 1 < argc) {
			std::string next_arg = arg[i + 1];
			if (next_arg.length() > 1 && next_arg[0] == '=' && next_arg[1] != '=') {
				// Handle "tw=3" -> "tw = 3"
				_all_arg += current_arg + "\n" + "=" + "\n" + next_arg.substr(1);
				i++; // Skip next argument since we processed it
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
		
		if (i != argc - 1) {
			_all_arg += "\n";
		} else {
			_all_arg += "\n;";
		}
	}

	// Generating the temp file pFile
	std::FILE *pFile;
	pFile = tmpfile(); // c++ function for generating a temporary file

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
	result_arg = command_parse(result_arg, width_type, width_value);

	fclose(command_in); // closing command_in

	return 0;
}
