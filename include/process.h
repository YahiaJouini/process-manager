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
    unsigned long prev_usr_time;  // last user cpu time
    unsigned long prev_sys_time;  // last system cpu time

   public:
    int pid;
    std::string name;
    std::string state;  // idle,running,stopped,sleeping ...
    float cpu_usage;
    long mem_usage;

    Process(int pid);
    // needs prevtotal and currTotal ticks of the process to calculate usage
    // from cpu total ticks,
    void update_stats(unsigned long prev_total, unsigned long curr_total);
};

#endif
