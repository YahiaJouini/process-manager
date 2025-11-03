#include "../../include/format.h"

#include <iomanip>

// trucate string to fit in column width
std::string truncate_string(const std::string& str, size_t width) {
    if (str.length() <= width) {
        return str;
    }
    return str.substr(0, width - 3) + "...";
}

std::string format_memory(float mem_kb) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(1) << (mem_kb / 1024.0f);
    return oss.str();
}

std::string format_cpu(float cpu) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(1) << cpu;
    return oss.str();
}

std::string get_state_name(const std::string& state) {
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
