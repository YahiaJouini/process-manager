#include "../../include/system_monitor.h"

#include <sstream>
#include <vector>

SystemMonitor::SystemMonitor() : totalCpuTimeCurr(0), totalCpuTimePrev(0) {
    this->update_cpu_times();
    this->totalCpuTimePrev = this->totalCpuTimeCurr;
};

void SystemMonitor::update_cpu_times() {
    // system wide cpu statistics
    std::string content = this->reader.read_file("/proc/stat");

    // first line contains aggregate stats of all cpu cores combined
    std::istringstream stream(content);

    std::string cpuLabel;
    unsigned long user, nice, system, idle, iowait, irq, softirq, steal;

    // store time spent by CPU in various states since the system booted
    // (idle,nice,user,system..)
    stream >> cpuLabel >> user >> nice >> system >> idle >> iowait >> irq >>
        softirq >> steal;

    // total = sum of all cpu times
    this->totalCpuTimeCurr =
        user + nice + system + idle + iowait + irq + softirq + steal;
}

void SystemMonitor::update_processes() {
    this->update_cpu_times();
    // list of process directories
    std::vector<std::string> pids = this->reader.list_directories("/proc");
    std::map<int, Process> new_map;

    // re-use existing processes or create new
    for (const auto& pid_str : pids) {
        int pid = std::stoi(pid_str);
        if (this->process_map.find(pid) != this->process_map.end()) {
            // re-use existing process
            Process& proc = this->process_map.find(pid);
            proc.update_stats(this->totalCpuTimePrev, this->totalCpuTimeCurr);
            new_map[pid] = proc;
        } else {
            // new process
            Process proc(pid);
            proc.update_stats(this->totalCpuTimePrev, this->totalCpuTimePrev);
            new_map[pid] = proc;
        }

        this->process_map = new_map;
        this->totalCpuTimePrev = this->totalCpuTimeCurr;
    }
}

std::vector<Process> SystemMonitor::get_processes() {
    std::vector<Process> procs;
    for (const auto& pair : this->process_map) {
        procs.push_back(pair.second);
    }
    return procs;
}
