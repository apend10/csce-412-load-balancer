#include "LoadBalancer.h"
#include "Request.h"
#include <sstream>

#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"

LoadBalancer::LoadBalancer() { }

void LoadBalancer::initialize(int numServers) {
    for(int i = 0; i < numServers; ++i) {
        servers.push_back(WebServer());
    }

    // Fill initial requests queue
    for(int i = 0; i < numServers * 100; ++i) {
        Request req = generator.generateRequest();
        addRequest(req);
    }
}

bool LoadBalancer::isBlocked(std::string ip) { 
    for (const std::string& pattern : blockedIPRanges) {
        // Find the asterisk position
        size_t asterisk = pattern.find('*');
        
        if (asterisk != std::string::npos) {
            // Compare only the part before the asterisk
            std::string prefix = pattern.substr(0, asterisk);
            if (ip.substr(0, prefix.length()) == prefix) {
                return true;  // IP matches this blocked pattern
            }
        } else {
            // No asterisk, exact match
            if (ip == pattern) {
                return true;
            }
        }
    }
    return false;  // No matches found
}

void LoadBalancer::addRequest(Request req) { 
    if(isBlocked(req.ip_in)) {
        totalRequestsBlocked++;  // ADD THIS
    } else {
        requestQueue.push(req);
    }
}

//main tick method
void LoadBalancer::update(int clock, std::ofstream& logFile) {
    updateServers();
    tryGenerateRequest(clock);
    assignRequestsToServers();
    checkScaling(clock, logFile);  // Only logs when scaling happens
    
    // Log summary every 1000 cycles
    if (clock % 1000 == 0) {
        logFile << BLUE << "[STATUS] Clock " << clock 
                << ": Queue=" << requestQueue.size() 
                << ", Servers=" << servers.size() 
                << ", Processed=" << totalRequestsProcessed << RESET << std::endl;
    }
}

void LoadBalancer::updateServers() {
    for (WebServer& ws : servers) { 
        if(ws.tick()) {
            totalRequestsProcessed++;
        }
    }
}

void LoadBalancer::assignRequestsToServers() {
    while (!requestQueue.empty()) {
        // Find an idle server
        bool assigned = false;
        for (WebServer& ws : servers) {
            if (ws.isIdle()) {
                Request req = requestQueue.front();  // Get the request
                requestQueue.pop();                  // Remove it from queue
                ws.assignRequest(req);               // Assign to server
                assigned = true;
                break;  // Move to next request
            }
        }
        
        // If no idle servers, stop trying
        if (!assigned) {
            break;
        }
    }
}

void LoadBalancer::tryGenerateRequest(int clock) { 
    if (clock % 5 == 0) {  // Every 5 cycles
        Request req = generator.generateRequest();
        addRequest(req);
    }
}

void LoadBalancer::checkScaling(int clock, std::ofstream& logFile) {
    if (clock - lastScaleTime < scalingWaitTime) return;
    
    int qSize = requestQueue.size();
    int nServers = servers.size();
    
    if (qSize < minQueueThreshold * nServers && nServers > 1) {
        servers.pop_back();
        lastScaleTime = clock;
        timesScaledDown++;
        logFile << RED << "[SCALE DOWN] Clock " << clock 
                << ": Removed server. Now " << servers.size() << " servers." << RESET << std::endl;
                
    } else if (qSize > maxQueueThreshold * nServers) {
        servers.push_back(WebServer());
        lastScaleTime = clock;
        timesScaledUp++;
        logFile << GREEN << "[SCALE UP] Clock " << clock 
                << ": Added server. Now " << servers.size() << " servers." << RESET << std::endl;
    }
}

void LoadBalancer::printStatistics() {
    std::cout << BLUE << "=== Load Balancer Statistics ===" << RESET << std::endl;
    std::cout << GREEN << "Requests Processed: " << totalRequestsProcessed << RESET << std::endl;
    std::cout << RED << "Requests Blocked: " << totalRequestsBlocked << RESET << std::endl;
    std::cout << YELLOW << "Times Scaled Up: " << timesScaledUp << RESET << std::endl;
    std::cout << YELLOW << "Times Scaled Down: " << timesScaledDown << RESET << std::endl;
    std::cout << MAGENTA << "Final Servers: " << servers.size() << RESET << std::endl;
    std::cout << "Final Queue Size: " << requestQueue.size() << std::endl;
}

void LoadBalancer::logState(int clock, std::ofstream& logFile) {
    logFile << "Clock: " << clock 
            << " | Queue: " << requestQueue.size()
            << " | Servers: " << servers.size() << std::endl;
}

void LoadBalancer::loadConfig(const std::string& filename) {
    std::ifstream file(filename);
    std::string line;
    
    while (std::getline(file, line)) {
        size_t pos = line.find('=');
        if (pos == std::string::npos) continue;
        
        std::string key = line.substr(0, pos);
        std::string value = line.substr(pos + 1);
        
        if (key == "SCALING_WAIT_TIME") {
            scalingWaitTime = std::stoi(value);
        } else if (key == "MIN_QUEUE_THRESHOLD") {
            minQueueThreshold = std::stoi(value);
        } else if (key == "MAX_QUEUE_THRESHOLD") {
            maxQueueThreshold = std::stoi(value);
        } else if (key == "BLOCKED_IP_RANGES") {
            blockedIPRanges.clear();
            std::stringstream ss(value);
            std::string range;
            while (std::getline(ss, range, ',')) {
                blockedIPRanges.push_back(range);
            }
        }
    }
    file.close();
}