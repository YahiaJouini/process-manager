#include "../../include/display.h"

#include <algorithm>
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/node.hpp>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#include "../../include/format.h"
#include "format.h"
#include "process.h"
#include "system_monitor.h"

using namespace ftxui;

Element Display::table_header() {
    return hbox({text(" PID ") | bold | color(Color::Cyan) | center |
                     size(WIDTH, EQUAL, 10),
                 separator(),
                 text(" PROGRAM ") | bold | color(Color::Cyan) | center |
                     size(WIDTH, EQUAL, 28),
                 separator(),
                 text(" CPU% ") | bold | color(Color::Cyan) | center |
                     size(WIDTH, EQUAL, 12),
                 separator(),
                 text(" MEMORY(MB) ") | bold | color(Color::Cyan) | center |
                     size(WIDTH, EQUAL, 14),
                 separator(),
                 text(" STATE ") | bold | color(Color::Cyan) | center |
                     size(WIDTH, EQUAL, 12)}) |
           bgcolor(Color::GrayDark);
}

Element Display::table_row(const Process& process, bool selected) {
    auto pid_text = text(" " + std::to_string(process.pid) + " ") | center |
                    size(WIDTH, EQUAL, 10);
    auto name_text = text(" " + truncate_string(process.name, 24) + " ") |
                     center | size(WIDTH, EQUAL, 28);
    auto cpu_text = text(" " + format_cpu(process.cpu_usage) + " ") | center |
                    size(WIDTH, EQUAL, 12);
    auto mem_text = text(" " + format_memory(process.mem_usage) + " ") |
                    center | size(WIDTH, EQUAL, 14);
    auto state_text = text(" " + get_state_name(process.state) + " ") | center |
                      size(WIDTH, EQUAL, 12);

    // color values based on usage

    if (process.cpu_usage > 50.0f) {
        cpu_text = cpu_text | color(Color::Red);
    } else if (process.cpu_usage > 20.0f) {
        cpu_text = cpu_text | color(Color::Yellow);
    } else {
        cpu_text = cpu_text | color(Color::Green);
    }

    if (process.mem_usage > 1000.0f) {
        mem_text = mem_text | color(Color::Red);
    } else if (process.mem_usage > 500.0f) {
        mem_text = mem_text | color(Color::Yellow);
    } else {
        mem_text = mem_text | color(Color::Green);
    }

    Element row = hbox({pid_text, separator(), name_text, separator(), cpu_text,
                        separator(), mem_text, separator(), state_text});

    if (selected) {
        return row | bgcolor(Color::Blue) | bold;
    }

    return row;
}
void Display::render() {
    using namespace ftxui;
    auto screen = ScreenInteractive::FitComponent();

    int selected = 0;
    bool show_info = false;

    SystemMonitor monitor;

    // lock to make sure only one thread touches a shared resource at a time
    std::mutex proc_mutex;
    std::atomic<bool> running = true;

    // initial load
    monitor.update_processes();
    std::vector<Process> processes = monitor.get_processes();

    auto table_renderer = Renderer([&] {
        // prevents race conditions
        std::lock_guard<std::mutex> lock(proc_mutex);
        Elements rows;
        rows.push_back(text("PROCESS MANAGER") | bold |
                       color(Color::GreenLight) | center);
        rows.push_back(separator());
        rows.push_back(this->table_header());
        rows.push_back(separator());

        // to ensure selected is not out of range if processes list size changes
        if (selected >= (int)processes.size()) {
            selected = (int)processes.size() - 1;
        }
        if (selected < 0) selected = 0;

        for (size_t i = 0; i < processes.size(); i++) {
            const Process& process = processes[i];
            rows.push_back(this->table_row(process, selected == (int)i));
        }
        Element main_table = vbox(rows) | border;

        if (show_info && !processes.empty() && selected >= 0 &&
            selected < (int)processes.size()) {
            const Process& p = processes[selected];
            Element info_box =
                vbox(
                    {text(" PROCESS DETAILS ") | bold | color(Color::Cyan) |
                         center,
                     separator(),
                     hbox({text(" PID: ") | bold, text(std::to_string(p.pid))}),
                     hbox({text(" Name: ") | bold, text(p.name)}),
                     hbox({text(" CPU: ") | bold,
                           text(format_cpu(p.cpu_usage) + "%")}),
                     hbox({text(" Memory: ") | bold,
                           text(format_memory(p.mem_usage) + " MB")}),
                     hbox({text(" State: ") | bold,
                           text(get_state_name(p.state))}),
                     separator(),
                     text(" Press 'i' to close ") | dim | center}) |
                border | color(Color::White) | bgcolor(Color::GrayDark);
            return vbox({main_table, text(""), info_box});
        }

        Element footer = hbox({text(" ↑/↓: Navigate ") | dim, separator(),
                               text(" i: Info ") | dim, separator(),
                               text(" q: Quit ") | dim}) |
                         center;
        return vbox({main_table, text(""), footer});
    });

    auto table_component = CatchEvent(table_renderer, [&](Event event) {
        if (event == Event::ArrowUp) {
            if (selected > 0) selected--;
            return true;
        } else if (event == Event::ArrowDown) {
            // lock to safely read processes.size()
            std::lock_guard<std::mutex> lock(proc_mutex);
            if (selected < (int)processes.size() - 1) selected++;
            return true;
        } else if (event == Event::Character('i') ||
                   event == Event::Character('I')) {
            show_info = !show_info;
            return true;
        } else if (event == Event::Character('q') ||
                   event == Event::Character('Q')) {
            running = false;  // signal the updater thread to stop
            screen.Exit();
            return true;
        }
        return false;
    });

    // background thread for live updates
    std::thread updater([&] {
        while (running) {
            {
                // Lock before writing to shared data
                std::lock_guard<std::mutex> lock(proc_mutex);
                monitor.update_processes();
                processes = monitor.get_processes();
                std::sort(processes.begin(), processes.end(),
                          [](const Process& a, const Process& b) {
                              return a.cpu_usage > b.cpu_usage;
                          });
            }
            screen.PostEvent(Event::Custom);  // trigger ftxui to redraw
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    });
    // Start the ftxui event loop.
    screen.Loop(table_component);

    // clean up on screen exit
    running = false;  // ensure running is false in case loop exited differently
    if (updater.joinable()) {
        updater.join();
    }
}
