// SPDX-License-Identifier: MIT
// Copyright (c) 2020-2026 Sam Urmian, Mateus de Oliveira Oliveira, and contributors.

#ifndef TREEWIDZARD_ERRORCONTROLLER_H
#define TREEWIDZARD_ERRORCONTROLLER_H

#include <iostream>

class ErrorController {
private:

public:
    void error_handle(std::string err, int errno);
};


#endif //TREEWIDZARD_ERRORCONTROLLER_H
