#include "../../include/process.h"

#include <iostream>
#include <sstream>
#include <string>
#include <thread>

Process::Process(int pid)
    : pid(pid),
      cpu_usage(0.0f),
      mem_usage(0),
      prev_usr_time(0),
      prev_sys_time(0) {};

void Process::update_stats(unsigned long prev_total, unsigned long curr_total) {
    const std::string stat_path =
        "/proc/" + std::to_string(this->pid) + "/stat";
    const std::string content = this->reader.read_file(stat_path);
    if (content.empty()) return;

    // because process name can contain spaces and is enclosed in parentheses
    size_t last_paren = content.rfind(')');
    if (last_paren == std::string::npos) return;

    // remove parentheses from name
    size_t first_paren = content.find('(');
    if (first_paren != std::string::npos) {
        this->name =
            content.substr(first_paren + 1, last_paren - first_paren - 1);
    }

    // will allow us to iterate content word by word
    // it seperates content by white spaces and starts from after last
    // parenthesis
    std::istringstream stream(content.substr(last_paren + 2));
    // retrieve process state
    stream >> this->state;

    int skip;
    // skip to the fields we need (field 14 utime and 15 stime)
    for (int i = 0; i < 10; i++) {
        stream >> skip;
    }

    unsigned long utime, stime;
    stream >> utime >> stime;

    // calculate cpu usage of process
    if (prev_total > 0) {
        // how much time this process used
        unsigned long processTimeUsed =
            (utime + stime) - (this->prev_usr_time + this->prev_sys_time);

        // how mych time passed for everyone (all processes)
        // currTotal and prevTotal will be passed from system monitor
        unsigned long totalTimePassed = curr_total - prev_total;

        if (totalTimePassed > 0) {
            // number of cpu cores
            static int num_cores = std::thread::hardware_concurrency();

            this->cpu_usage =
                (100.0f * processTimeUsed) /
                (totalTimePassed *
                 num_cores);  // normalize by number of cores of the cpu
        }
    }

    this->prev_usr_time = utime;
    this->prev_sys_time = stime;

    // get memory usage from /proc/<pid>/status
    const std::string status_path =
        "/proc/" + std::to_string(this->pid) + "/status";
    const std::string status_content = this->reader.read_file(status_path);

    // ex: VmRSS: 9200kB
    size_t pos = status_content.find("VmRSS:");
    if (pos != -1) {
        // pos + 6 because between "VmRSS: val" is 6 spaces
        std::istringstream memory_stream(status_content.substr(pos + 6));
        memory_stream >> this->mem_usage;
    }
}
