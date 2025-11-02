#ifndef PROCESS_H
#define PROCESS_H

#include <string>

class Process {
   private:
    int pid;
    std::string name;
    std::string state;
    float cpuUsage;
    long memUsage;
    int ppid;  // parent ID

   public:
    Process(int pid);
    std::string toString();
    bool kill(int pid);

    // needs prevtotal and currTotal ticks of the process to calculate usage
    // from cpu total ticks,
    void updateStats(unsigned long prevTotal, unsigned int currTotal);
};

#endif
