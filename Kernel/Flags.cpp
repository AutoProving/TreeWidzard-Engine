// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.

#include "Flags.h"
Flags::Flags() {
	content.insert({"printStates", 0});		// print all states
	content.insert({"loopTime", 0});		// time for loop
	content.insert({"createStateTime", 0}); // time to create a state
	content.insert({"stateSize", 0});		// prints size of state
}

float Flags::get(string s)
{
    if(content.find(s)!=content.end()){
        return content[s];
    }else{
        return -1;
    }
}

void Flags::add_flag(string s, float value) {
	if (content.find(s) != content.end()) {
		cout << "Error: flag already set!" << endl;
		exit(1);
	}
	content[s] = value;
}
