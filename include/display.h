#ifndef DISPLAY_H
#define DISPLAY_H

#include <ftxui/component/component.hpp>

#include "ftxui/dom/elements.hpp"
#include "process.h"

class Display {
   public:
    ftxui::Element table_header();
    ftxui::Element table_row(const Process&, bool selected);
    ftxui::Element info_box();
    void render();
};

#endif
