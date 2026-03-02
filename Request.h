#ifndef REQUEST_H
#define REQUEST_H

#include <string>
#include <stdexcept>

struct Request {
    std::string ip_in;
    std::string ip_out;
    int cyclesRequired;
    char job_type;

    Request (std::string& in, std::string& out, int time, char type) : ip_in(in), ip_out(out), cyclesRequired(time), job_type(type) {
        if (time <= 0) {
            throw std::invalid_argument("Time (cyclesRequired) must be > 0.");
        }

        if (type != 'S' && type != 'P') {
            throw std::invalid_argument("Job type must be 'S' or 'P'.");
        }
    }

};

#endif // REQUESTS_H
