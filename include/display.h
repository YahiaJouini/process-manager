#ifndef DISPLAY_H
#define DISPLAY_H

#include <vector>

#include "process.h"

class Display {
   public:
    void render(const std::vector<Process>&);
    void clear_screen();
};

#endif
