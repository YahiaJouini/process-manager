#ifndef SYSTEM_MONITOR_H
#define SYSTEM_MONITOR_H

#include <map>
#include <vector>

#include "file_reader.h"
#include "process.h"

class SystemMonitor {
   private:
    FileReader reader;
    std::map<int, Process> process_map;
    unsigned long totalCpuTimePrev;
    unsigned long totalCpuTimeCurr;
    void update_cpu_times();

   public:
    SystemMonitor();
    void update_processes();
    std::vector<Process> get_processes();
    void refresh();
};

#endif
