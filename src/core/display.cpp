#include "../../include/display.h"

#include <algorithm>
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/node.hpp>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

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
    using namespace ftxui;

    auto screen = ScreenInteractive::FitComponent();

    int selected = 0;
    // to show info about highlighted process
    bool show_info = false;

    auto table_renderer = Renderer([&] {
        Elements rows;

        rows.push_back(hbox({text("PID") | bold | border,
                             text("PROGRAM") | bold | border | flex,
                             text("CPU%") | bold | border,
                             text("MEMORY(MB)") | bold | border,
                             text("STATE") | bold | border}));

        for (int i = 0; i < processes.size(); i++) {
            const Process& p = processes[i];
            auto style = (i == selected) ? inverted : nothing;

            std::ostringstream s_stream;

            s_stream << std::setw(8) << p.pid << std::setw(26) << p.name
                     << std::setw(10) << p.cpu_usage << std::setw(12)
                     << p.mem_usage << std::setw(10) << p.state;

            rows.push_back(text(s_stream.str()) | style);
        }

        auto main_table = vbox(rows);

        if (show_info) {
            auto& p = processes[selected];
            auto info_box =
                vbox({text("PROCESS INFO") | bold,
                      text("PID: " + std::to_string(p.pid)),
                      text("Name: " + p.name),
                      text("CPU: " + std::to_string(p.cpu_usage)),
                      text("Memory: " + std::to_string(p.mem_usage)),
                      text("State: " + p.state),
                      text("Press 'i' to close info")}) |
                border | size(HEIGHT, LESS_THAN, 10);

            return vbox({main_table, separator(), info_box});
        }

        return main_table;
    });

    auto table_component = CatchEvent(table_renderer, [&](Event event) {
        if (event == Event::ArrowUp) {
            if (selected > 0) selected--;
        } else if (event == Event::ArrowDown) {
            if (selected < (int)processes.size() - 1) selected++;
        } else if (event == Event::Character('i')) {
            show_info = !show_info;
        } else if (event == Event::Character('q')) {
            screen.Exit();
        }
        return true;
    });

    screen.Loop(table_component);
}
