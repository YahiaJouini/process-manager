#include "../../include/process.h"

#include <sstream>
#include <string>

Process::Process(int pid)
    : pid(pid), cpuUsage(0.0f), memUsage(0), prevUtime(0), prevStime(0) {
    updateStats(0, 1);
};

void Process::updateStats(unsigned long prevTotal, unsigned long currTotal) {
    const std::string stat_path = "/proc" + std::to_string(this->pid) + "/stat";
    const std::string content = this->reader.readFile(stat_path);
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
            (utime + stime) - (this->prevUtime + this->prevStime);

        // how mych time passed for everyone (all processes)
        // currTotal and prevTotal will be passed from system monitor
        unsigned long totalTimePassed = currTotal - prevTotal;

        if (totalTimePassed > 0) {
            this->cpuUsage = (100.0f * processTimeUsed) / totalTimePassed;
        }
    }

    this->prevUtime = utime;
    this->prevStime = stime;

    // get memory usage from /proc/<pid>/status
    const std::string status_path =
        "/proc" + std::to_string(this->pid) + "/status";
    const std::string status_content = this->reader.readFile(status_path);

    // ex: VmRSS: 9200kB
    size_t pos = status_content.find("VmRSS:");
    if (pos != -1) {
        // pos + 6 because between "VmRSS: val" is 6 spaces
        std::istringstream memory_stream(status_content.substr(pos + 6));
        memory_stream >> this->memUsage;
    }
}
