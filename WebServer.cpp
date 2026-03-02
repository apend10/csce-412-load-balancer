#include "WebServer.h"
#include <stdexcept>

WebServer::WebServer() : cycles_remaining(0) { }

void WebServer::assignRequest(Request req) { 
    if (current_request.has_value()) {
        throw std::runtime_error("WebServer is already processing a request.");
    }
    current_request = req;
    cycles_remaining = req.cyclesRequired;
}

bool WebServer::tick() { 
    if (current_request.has_value()) {
        cycles_remaining--;
        if (cycles_remaining == 0) {
            current_request.reset(); // Finish the request
            return true; // Return true to indicate completion
        }
    }
    return false;
}

bool WebServer::isIdle() { 
    return !current_request.has_value();
}