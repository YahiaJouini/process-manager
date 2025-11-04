#include "../../include/display.h"

#include <signal.h>  // for kill and SIGKILL

#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/node.hpp>
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
                     size(WIDTH, EQUAL, 12)});
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
        return row | bgcolor(Color::Blue) | bold | focus;
    }

    return row;
}

Element Display::info_box(const Process& p) {
    return vbox({text(" PROCESS DETAILS ") | bold | color(Color::Cyan) | center,
                 separator(),
                 hbox({text(" PID: ") | bold, text(std::to_string(p.pid))}),
                 hbox({text(" Name: ") | bold, text(p.name)}),
                 hbox({text(" CPU: ") | bold,
                       text(format_cpu(p.cpu_usage) + " %")}),
                 hbox({text(" Memory: ") | bold,
                       text(format_memory(p.mem_usage) + " MB")}),
                 hbox({text(" State: ") | bold, text(get_state_name(p.state))}),
                 separator(), text(" Press 'i' to close ") | dim | center}) |
           border | color(Color::White) | bgcolor(Color::GrayDark);
}

Element Display::footer() {
    return hbox({text(" ↑/↓: Navigate ") | dim, separator(),
                 text(" i: Info ") | dim, separator(), text(" k: Kill ") | dim,
                 separator(), text(" s: Sort ") | dim, separator(),
                 text(" q: Quit ") | dim}) |
           center;
}

Element Display::kill_confirm_popup(const Process& process) {
    return vbox({
               text("Kill Process?") | bold | color(Color::RedLight) | center,
               separator(),
               text("PID: " + std::to_string(process.pid)) | center,
               text("  Name: " + process.name + "  ") | center,
               separator(),
               text(" Type 'y' to confirm, ESC or 'n' to cancel ") | center,
           }) |
           borderRounded | size(WIDTH, LESS_THAN, 50) |
           size(HEIGHT, LESS_THAN, 10) | clear_under | bgcolor(Color::Black) |
           center;
}

Element Display::sort_popup(int sort_selected) {
    std::vector<std::string> options = {"CPU Usage", "Memory Usage"};
    Elements items;
    for (int i = 0; i < (int)options.size(); ++i) {
        bool selected_item = (i == sort_selected);
        items.push_back(text(options[i]) |
                        (selected_item ? inverted : nothing));
    }
    return vbox({text(" Sort by ") | bold | center, separator(),
                 vbox(items) | center | size(WIDTH, EQUAL, 20)}) |
           borderRounded | clear_under | bgcolor(Color::Black) | center;
}

void Display::render() {
    auto screen = ScreenInteractive::FitComponent();
    int selected = 0;  // position of selected process
    bool show_info = false;
    bool show_sort_mode = false;
    int sort_selected = 0;  // 0 = cpu, 1 = momory
    bool show_kill_confirm = false;

    SystemMonitor monitor;

    // single mutex to protect shared state (processes, sort_by, and UI state)
    std::mutex state_mutex;

    // protected by state mutex
    SystemMonitor::SortBy sort_by = SystemMonitor::SortBy::Cpu;
    std::atomic<bool> running = true;

    // initial load
    monitor.update_processes();
    std::vector<Process> processes = monitor.get_processes(sort_by);

    auto table_renderer = Renderer([&] {
        // lock state through the entire render so only render thread has access
        std::lock_guard<std::mutex> lock(state_mutex);

        Elements rows;
        rows.push_back(text("PROCESS MANAGER") | bold |
                       color(Color::GreenLight) | center);
        rows.push_back(separator());
        rows.push_back(this->table_header());
        rows.push_back(separator());

        int p_size = (int)processes.size();

        // to ensure selected is not out of range if processes list
        // size changes
        if (selected >= p_size) selected = p_size - 1;

        if (selected < 0) selected = 0;

        for (int i = 0; i < p_size; i++) {
            const Process& process = processes[i];
            rows.push_back(this->table_row(process, selected == i));
        }

        Element main_table = vbox(rows) | vscroll_indicator  // scrollbar
                             | frame  // make it scrollable
                             | flex   // make it expand to fill space
                             | border;

        // don't show footer on show info box
        if (show_info && selected >= 0 && selected < p_size) {
            return vbox(
                {main_table, text(""), this->info_box(processes[selected])});
        }

        Element layout = vbox({main_table, text(""), this->footer()});

        // overwrite popup if active
        if (show_sort_mode) {
            layout = dbox({layout, this->sort_popup(sort_selected)});
        }

        if (show_kill_confirm && selected >= 0 && selected < p_size) {
            layout =
                dbox({layout, this->kill_confirm_popup(processes[selected])});
        }
        return layout;
    });

    auto table_component = CatchEvent(table_renderer, [&](Event event) {
        // kill popup events
        if (show_kill_confirm) {
            if (event == Event::Character('y') ||
                event == Event::Character('Y')) {
                // lock process so only this thread can modify it
                {
                    std::lock_guard<std::mutex> lock(state_mutex);
                    if (selected >= 0 && selected < (int)processes.size()) {
                        kill(processes[selected].pid, SIGKILL);
                        // update to reflect changes
                        monitor.update_processes();
                        processes = monitor.get_processes(sort_by);
                    }
                }
                show_kill_confirm = false;
                screen.PostEvent(Event::Custom);  // force redraw right away
                return true;
            } else if (event.is_character() || event == Event::Escape) {
                // any character other than y
                show_kill_confirm = false;
                return true;
            }
            return false;
        }

        // sort popup events
        if (show_sort_mode) {
            if (event == Event::ArrowUp && sort_selected > 0) {
                sort_selected--;
                return true;
            } else if (event == Event::ArrowDown && sort_selected < 1) {
                sort_selected++;
                return true;
            } else if (event == Event::Return) {
                // lock state because we're modifying sort_by wich is shared
                // with updater thread
                {
                    std::lock_guard<std::mutex> lock(state_mutex);
                    if (sort_selected == 0) {
                        sort_by = SystemMonitor::SortBy::Cpu;
                    } else if (sort_selected == 1) {
                        sort_by = SystemMonitor::SortBy::Memory;
                    }
                    processes = monitor.get_processes(sort_by);
                }
                selected = 0;
                show_sort_mode = false;
                screen.PostEvent(Event::Custom);  // force redraw right away
                return true;
            } else if (event == Event::Escape) {
                show_sort_mode = false;
                return true;
            }
            return false;
        }
        // sort popup events
        if (show_sort_mode) {
            if (event == Event::ArrowUp && sort_selected > 0) {
                sort_selected--;
                return true;
            } else if (event == Event::ArrowDown && sort_selected < 1) {
                sort_selected++;
                return true;
            } else if (event == Event::Return) {
                // lock state because we're modifying sort_by wich is shared
                // with updater thread
                {
                    std::lock_guard<std::mutex> lock(state_mutex);
                    if (sort_selected == 0) {
                        sort_by = SystemMonitor::SortBy::Cpu;
                    } else if (sort_selected == 1) {
                        sort_by = SystemMonitor::SortBy::Memory;
                    }
                    processes = monitor.get_processes(sort_by);
                }
                selected = 0;
                show_sort_mode = false;
                screen.PostEvent(Event::Custom);  // force redraw right away
                return true;
            } else if (event == Event::Escape) {
                show_sort_mode = false;
                return true;
            }
            return false;
        }

        // allow events only if no popup is shown
        if (!show_kill_confirm && !show_sort_mode) {
            // regular table events
            if (event == Event::ArrowUp) {
                if (selected > 0) selected--;
                return true;
            } else if (event == Event::ArrowDown) {
                // lock to safely read processes.size()
                std::lock_guard<std::mutex> lock(state_mutex);
                if (selected < (int)processes.size() - 1) selected++;
                return true;
            } else if (event == Event::Character('i') ||
                       event == Event::Character('I')) {
                show_info = !show_info;
                return true;
            } else if (event == Event::Character('k') ||
                       event == Event::Character('K')) {
                show_kill_confirm = true;
                return true;
            } else if (event == Event::Character('s') ||
                       event == Event::Character('S')) {
                show_sort_mode = true;
                return true;
            } else if (event == Event::Character('q') ||
                       event == Event::Character('Q')) {
                running = false;  // signal the updater thread to stop
                screen.Exit();
                return true;
            }
        }
        return false;
    });

    // background thread for live updates
    std::thread updater([&] {
        while (running) {
            // added show_kill_confirm check to avoid updating while in kill
            // because updating process list might remove the selected process
            if (!show_kill_confirm) {
                // Lock before writing to shared data
                std::lock_guard<std::mutex> lock(state_mutex);
                monitor.update_processes();
                processes = monitor.get_processes(sort_by);
            }

            screen.PostEvent(Event::Custom);  // trigger ftxui to redraw
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    });

    // start the ftxui event loop.
    screen.Loop(table_component);

    // clean up on screen exit
    running = false;  // ensure running is false in case loop exited differently
    if (updater.joinable()) {
        updater.join();
    }
}
