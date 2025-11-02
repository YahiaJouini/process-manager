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
    void updateCpuTimes();

   public:
    SystemMonitor();
    void updateProcesses();
    std::vector<Process> getProcesses();
    void refresh();
};

#endif
