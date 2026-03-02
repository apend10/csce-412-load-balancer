#ifndef LoadBalancer_H
#define LoadBalancer_H

#include <queue>
#include <vector>    
#include <string> 
#include <iostream>
#include <fstream>

#include "WebServer.h"
#include "RequestGenerator.h"
#include "Request.h"

class LoadBalancer {
    private:
        std::vector<WebServer> servers;
        std::queue<Request> requestQueue;
        std::vector<std::string> blockedIPRanges = {"192.168.*", "10.0.0.*"};
        RequestGenerator generator;

        int lastScaleTime = 0;
        int scalingWaitTime = 100;
        int totalRequestsProcessed = 0;
        int totalRequestsBlocked = 0;
        int timesScaledUp = 0;
        int timesScaledDown = 0;

        int minQueueThreshold = 50;
        int maxQueueThreshold = 80;

    public:
        LoadBalancer();
        bool isBlocked(std::string ip);
        void update(int clock, std::ofstream& logFile); //main tick method
        void addRequest(Request req); //add to queue and IP check
        void tryGenerateRequest(int clock); //generates random requests
        void assignRequestsToServers(); //takes requests from queue and gives them to idle servers
        void updateServers(); //to call tick on all servers
        void checkScaling(int clock, std::ofstream& logFile); //add/remove servers based on queue size
        void initialize(int numServers); //set up initial servers
        void printStatistics(); //ouput stats
        void logState(int clock, std::ofstream& logFile);
        void loadConfig(const std::string& filename);

};

#endif