// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.
#include <stdio.h>
#include <iostream>
#include <fstream>
#include "Controller/Parser/command_parser.hpp"

using namespace std;
extern FILE *command_in;
using namespace std;
int main(int argc, char *arg[]) {
    std::string _all_arg;
    for (int i = 1 ; i < argc; i++) {
        _all_arg += arg[i];
        if(i != argc-1){
            _all_arg += "\n";
        }else{
            _all_arg +="\n;";
        }
    }
    //cout<<_all_arg;
    // Create and open a text file
    ofstream MyFile("command.txt");
    // Write to the file
    MyFile << _all_arg;
    // Close the file
    MyFile.close();
    command_in = fopen("command.txt", "r");
    if (!command_in) {
        std::perror("File opening failed");
        return EXIT_FAILURE;
    }
    int result_arg=10;
    result_arg = command_parse(result_arg);

	return 0;
}
