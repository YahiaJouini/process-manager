#ifndef DISPLAY_H
#define DISPLAY_H

#include <vector>

#include "process.h"

class Display {
   public:
    void render(const std::vector<Process>&);
    std::string truncate_string(const std::string&, size_t);
    std::string format_memory(float);
    std::string format_cpu(float);
    std::string get_state_name(const std::string&);
    void clear_screen();
};

#endif
