#include "../../include/process.h"

#include <sstream>
#include <string>

Process::Process(int pid)
    : pid(pid),
      cpu_usage(0.0f),
      mem_usage(0),
      prev_usr_time(0),
      prev_sys_time(0) {
    update_stats(0, 1);
};

void Process::update_stats(unsigned long prevTotal, unsigned long currTotal) {
    const std::string stat_path = "/proc" + std::to_string(this->pid) + "/stat";
    const std::string content = this->reader.read_file(stat_path);
    if (content.empty()) return;

    // will allow us to iterate content word by word
    // it seperates content by white spaces
    std::istringstream stream(content);

    int pid, skip;
    std::string pname;

    // retrieve process id
    stream >> pid;
    // retrieve process name
    stream >> name;

    // skip to the fields we need (field 14 utime and 15 stime)
    for (int i; i < 10; i++) {
        stream >> skip;
    }

    unsigned long utime, stime;
    stream >> utime >> stime;

    // calculate cpu usage of process
    if (prevTotal > 0) {
        // how much time this process used
        unsigned long processTimeUsed =
            (utime + stime) - (this->prev_usr_time + this->prev_sys_time);

        // how mych time passed for everyone (all processes)
        // currTotal and prevTotal will be passed from system monitor
        unsigned long totalTimePassed = currTotal - prevTotal;

        if (totalTimePassed > 0) {
            this->cpu_usage = (100.0f * processTimeUsed) / totalTimePassed;
        }
    }

    this->prev_usr_time = utime;
    this->prev_sys_time = stime;

    // get memory usage from /proc/<pid>/status
    const std::string status_path =
        "/proc" + std::to_string(this->pid) + "/status";
    const std::string status_content = this->reader.read_file(status_path);

    // ex: VmRSS: 9200kB
    size_t pos = status_content.find("VmRSS:");
    if (pos != -1) {
        // pos + 6 because between "VmRSS: val" is 6 spaces
        std::istringstream memory_stream(status_content.substr(pos + 6));
        memory_stream >> this->mem_usage;
    }
}
