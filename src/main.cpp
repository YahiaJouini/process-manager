#include <chrono>
#include <csignal>
#include <thread>

#include "../include/display.h"
#include "../include/system_monitor.h"

bool running = true;
void signalHandler(int) { running = false; }

int main() {
    // SIGINT is signal sent when CTRL+C is pressed
    // handler is what get's called when that signal arrives
    signal(SIGINT, signalHandler);

    SystemMonitor monitor;
    Display display;

    while (running) {
        monitor.update_processes();
        display.render(monitor.get_processes());
        // update process info every 2 seconds
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }

    return 0;
}
