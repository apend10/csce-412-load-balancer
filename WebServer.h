#ifndef WebServer_H
#define WebServer_H

#include "WebServer.h"
#include "Request.h"
#include <optional>

class WebServer { 
    private:
        int cycles_remaining;
        std::optional<Request> current_request;

    public:
        WebServer();
        void assignRequest(Request req);  // Initiate processing of a request
        bool tick();                       // Decrement timer, finish if done
        bool isIdle();                     // Can I take more work?
};

#endif