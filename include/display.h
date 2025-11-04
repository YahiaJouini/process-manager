#ifndef DISPLAY_H
#define DISPLAY_H

#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>

#include "ftxui/dom/elements.hpp"
#include "process.h"

class Display {
   public:
    void render();

   private:
    ftxui::Element table_header();
    ftxui::Element table_row(const Process&, bool selected);
    ftxui::Element info_box(const Process&);
    ftxui::Element footer();
    ftxui::Element sort_popup(int);
    ftxui::Element kill_confirm_popup(const Process&);

    // helper
    bool in_range(int value, int min, int max) {
        return (value >= min && value <= max);
    }
};

#endif
