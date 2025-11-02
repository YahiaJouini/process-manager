#ifndef PROCESS_H
#define PROCESS_H

#include <string>

#include "file_reader.h"

class Process {
   private:
    FileReader reader;
    // cpu time = how much time cpu spent running this process
    // linux divides cpu time to : user time (executing instructions time), and
    // cpu time (time spent in system calls)
    unsigned long prevStime;  // last user cpu time
    unsigned long prevUtime;  // last system cpu time

   public:
    int pid;
    std::string name;
    std::string state;  // idle,running,stopped,sleeping ...
    float cpuUsage;
    long memUsage;

    Process(int pid);
    std::string toString();
    bool kill(int pid);
    // needs prevtotal and currTotal ticks of the process to calculate usage
    // from cpu total ticks,
    void updateStats(unsigned long prevTotal, unsigned long currTotal);
};

#endif
