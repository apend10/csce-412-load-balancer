#ifndef RequestGenerator_H
#define RequestGenerator_H

#include <random>
#include <string>
#include "Request.h"

class RequestGenerator {
    private:
        std::mt19937 gen;
        std::uniform_int_distribution<> ip_part;
        std::uniform_int_distribution<> proc_time;
        std::uniform_int_distribution<> job_type;
    public:
        RequestGenerator();
        Request generateRequest();

};

#endif 