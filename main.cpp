#include <cstdio>
#include <iostream>
#include <fstream>
#include <chrono>
#include <ctime>
#include <filesystem>

#include <unistd.h>
#include <sys/time.h>

#include "Controller/Parser/command_parser.hpp"


extern std::FILE *command_in;

namespace fs = std::filesystem;

int main(int argc, char *arg[]) {
    // Reading the input and put it in string _all_arg
    std::string _all_arg;
    for (int i = 1 ; i < argc; i++) {
        _all_arg += arg[i];
        if(i != argc-1){
            _all_arg += "\n";
        }else{
            _all_arg +="\n;";
        }
    }

    // Generating the temp file pFile
    char buffer [256];
    std::FILE * pFile;
    pFile = tmpfile(); // c++ function for generating a temporary file

    std::fputs(_all_arg.c_str(), pFile);  // Write _all_arg in pFile
    rewind(pFile); // Sets the position indicator associated with stream to the beginning of the file.
    command_in = pFile; // Set command_in equals to pFile


    /* Test to print the content of command_in
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
    int result_arg=10;
    std::string width_type;
    int width_value;
    result_arg = command_parse(result_arg,width_type,width_value);

    fclose(command_in); // closing command_in

    return 0;

}
