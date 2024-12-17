#include "ErrorController.h"

void ErrorController::error_handle(std::string err, int errno) {
    std::cout<<err;
    exit(errno);
}
