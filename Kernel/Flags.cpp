// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.

#include "Flags.h"
Flags::Flags() {
    content.insert({"printStates",0}); // print all states
    content.insert({"loopTime",0}); // time for loop
    content.insert({"createStateTime",0}); // time to create a state
    content.insert({"stateSize",0}); // prints size of state
}