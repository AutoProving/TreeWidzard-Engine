// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.
#include <stdio.h>
#include <iostream>
#include <fstream>
#include "Controller/Parser/command_parser.hpp"
#include <chrono>
#include <sys/time.h>
#include <ctime>

using namespace std;
extern FILE *command_in;
using namespace std;


using std::cout; using std::endl;
using std::chrono::duration_cast;
using std::chrono::milliseconds;
using std::chrono::seconds;
using std::chrono::system_clock;

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
    int iSecret;

    /* initialize random seed: */
    srand (time(NULL));

    /* generate secret number between 1 and 100: */
    iSecret = rand() % 100 + 1;
    auto millisec_since_epoch = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
    string s = "auxfiles/"+to_string(millisec_since_epoch)+"_"+to_string(iSecret)+".txt";
    cout<<s<<endl;
    ofstream MyFile(s);
    // Write to the file
    MyFile << _all_arg;
    // Close the file
    MyFile.close();
    command_in = fopen(s.c_str(), "r");
    if (!command_in) {
        std::perror("File opening failed");
        return EXIT_FAILURE;
    }
    int result_arg=10;
    string width_type;
    int width_value;
    result_arg = command_parse(result_arg,width_type,width_value);

	return 0;
}
