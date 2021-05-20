#include "ErrorController.h"

void ErrorController::error_handle(string err, int errno) {
    cout<<err;
    exit(errno);
}
