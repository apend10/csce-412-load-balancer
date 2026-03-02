#include "LoadBalancer.h"
#include <iostream>
#include <fstream>

int main() {
    int simulation_time, num_servers;
    
    std::cout << "Enter number of servers: ";
    std::cin >> num_servers;
    std::cout << "Enter simulation time: ";
    std::cin >> simulation_time;
    
    LoadBalancer lb;
    lb.loadConfig("config.txt"); 

    lb.initialize(num_servers); 
    
    // Create log file
    std::ofstream logFile("simulation.log");
    
    for(int i = 0; i < simulation_time; ++i) {
        lb.update(i, logFile);  // Pass logFile to update
    }
    
    logFile.close();
    lb.printStatistics();
    
    return 0;
}