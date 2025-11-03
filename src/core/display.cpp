#include "../../include/display.h"

#include <algorithm>
#include <iomanip>
#include <iostream>

void Display::clear_screen() { std::cout << "\033[2J\033[1;1H"; };

void Display::render(const std::vector<Process>& processes) {
    this->clear_screen();

    std::cout << "TASK MANAGER\n";
    std::cout << "===============================================\n";
    std::cout << std::left << std::setw(8) << "PID" << std::setw(25) << "NAME"
              << std::setw(10) << "CPU%" << std::setw(12) << "MEMORY(KB)"
              << std::setw(8) << "STATE"
              << "\n";
    std::cout << "===============================================\n";

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

        std::cout << std::left << std::setw(8) << proc.pid << std::setw(25)
                  << proc.name.substr(0, 24) << std::setw(10) << std::fixed
                  << std::setprecision(1) << proc.cpu_usage << std::setw(12)
                  << proc.mem_usage << std::setw(8) << proc.state << "\n";
    }

    std::cout << "\nCtrl+C to exit\n";
}
