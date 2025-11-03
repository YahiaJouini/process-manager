#include <csignal>

#include "../include/display.h"

bool running = true;
void signalHandler(int) { running = false; }

int main() {
    // SIGINT is signal sent when CTRL+C is pressed
    // handler is what get's called when that signal arrives
    signal(SIGINT, signalHandler);

    Display display;
    display.render();
    return 0;
}
