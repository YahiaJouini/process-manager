#ifndef DISPLAY_H
#define DISPLAY_H

#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>

#include "ftxui/dom/elements.hpp"
#include "process.h"

class Display {
   public:
    ftxui::Element table_header();
    ftxui::Element table_row(const Process&, bool selected);
    ftxui::Element info_box(const Process&);
    ftxui::Element footer();
    ftxui::Element sort_popup(int);
    void render();
};

#endif
