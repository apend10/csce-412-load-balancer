#include "Request.h"
#include "RequestGenerator.h"

RequestGenerator::RequestGenerator() : 
    gen(std::random_device{}()), 
    ip_part(0, 255), 
    proc_time(5,50), 
    job_type(0,1)
{

}

Request RequestGenerator::generateRequest() {
    // Force 20% of IPs to be blocked for testing
    std::string ip_in;
    if (job_type(gen) == 0 && ip_part(gen) < 51) {  // ~20% chance
        ip_in = "192.168.1." + std::to_string(ip_part(gen));  // Blocked range
    } else {
        ip_in = std::to_string(ip_part(gen)) + "." +
                std::to_string(ip_part(gen)) + "." +
                std::to_string(ip_part(gen)) + "." +
                std::to_string(ip_part(gen));
    }
    
    std::string ip_out = std::to_string(ip_part(gen)) + "." +
                         std::to_string(ip_part(gen)) + "." +
                         std::to_string(ip_part(gen)) + "." +
                         std::to_string(ip_part(gen));

    int time = proc_time(gen);
    char type = (job_type(gen) == 0) ? 'P' : 'S';

    Request req(ip_in, ip_out, time, type);
    return req;
}
