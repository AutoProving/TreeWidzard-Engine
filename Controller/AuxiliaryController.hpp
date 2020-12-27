

#ifndef TREEWIDZARD_AUXILIARYCONTROLLER_HPP
#define TREEWIDZARD_AUXILIARYCONTROLLER_HPP
#include <iostream>
#include <fstream>
void show_manual(){
    std::ifstream f("../manual.txt");
    if (f.is_open())
        std::cout << f.rdbuf();
    exit(20);
}

#endif //TREEWIDZARD_AUXILIARYCONTROLLER_HPP
