#include "../../include/display.h"

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <sstream>

void Display::clear_screen() { std::cout << "\033[2J\033[1;1H"; }

// trucate string to fit in column width
std::string Display::truncate_string(const std::string& str, size_t width) {
    if (str.length() <= width) {
        return str;
    }
    return str.substr(0, width - 3) + "...";
}

std::string Display::format_memory(float mem_kb) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(1) << (mem_kb / 1024.0f);
    return oss.str();
}

std::string Display::format_cpu(float cpu) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(1) << cpu;
    return oss.str();
}

std::string Display::get_state_name(const std::string& state) {
    if (state.empty()) return "Unknown";

    switch (state[0]) {
        case 'R':
            return "Running";
        case 'S':
            return "Sleeping";
        case 'D':
            return "Disk Sleep";
        case 'Z':
            return "Zombie";
        case 'T':
            return "Stopped";
        case 't':
            return "Tracing";
        case 'W':
            return "Waking";
        case 'X':
            return "Dead";
        case 'x':
            return "Dead";
        case 'K':
            return "Wakekill";
        case 'P':
            return "Parked";
        case 'I':
            return "Idle";
        default:
            return state;
    }
}

void Display::render(const std::vector<Process>& processes) {
    this->clear_screen();

    std::cout << "PROCESS MANAGER\n";
    std::cout
        << "================================================================\n";
    std::cout << std::left << std::setw(8) << "PID" << std::setw(26)
              << "PROGRAM" << std::setw(10) << "CPU%" << std::setw(12)
              << "MEMORY(MB)" << std::setw(10) << "STATE"
              << "\n";
    std::cout
        << "================================================================\n";

    // sort by cpu usage
    std::vector<Process> sorted = processes;
    std::sort(sorted.begin(), sorted.end(),
              [](const Process& a, const Process& b) {
                  return a.cpu_usage > b.cpu_usage;
              });

    // show first 25
    int count = 0;
    for (const auto& proc : sorted) {
        if (count++ >= 25) break;

        std::cout << std::left << std::setw(8) << proc.pid << std::setw(26)
                  << truncate_string(proc.name, 25) << std::setw(10)
                  << format_cpu(proc.cpu_usage) << std::setw(12)
                  << format_memory(proc.mem_usage) << std::setw(10)
                  << get_state_name(proc.state) << "\n";
    }

    std::cout << std::endl << "Ctrl+C to exit" << std::endl;
}
