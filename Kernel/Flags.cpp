// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.

#include "Flags.h"
Flags::Flags() {
    content.insert({"printStates",0}); // print all states
    content.insert({"loopTime",0}); // time for loop
    content.insert({"createStateTime",0}); // time to create a state
    content.insert({"stateSize",0}); // prints size of state
}

int Flags::get(string s) {
	return content[s];
}

void Flags::add_flag(string s, int value) {
	if (content.find(s) != content.end()) {
		cout << "Error: flag already set!" << endl;
		exit(1);
	}
	content[s] = value;
}
