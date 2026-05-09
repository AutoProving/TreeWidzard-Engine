// SPDX-License-Identifier: MIT
// Copyright (c) 2020-2026 Sam Urmian, Mateus de Oliveira Oliveira, and contributors.

#include "ErrorController.h"

void ErrorController::error_handle(std::string err, int errno) {
    std::cout<<err;
    exit(errno);
}
