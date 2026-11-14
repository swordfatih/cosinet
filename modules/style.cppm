module;

#include <centui/types.hpp>

export module cosinet.style;

export namespace cosinet
{

struct overlay
{
    centui::length width = centui::percent(100);
    centui::length height = centui::percent(100);
    centui::edges padding = centui::edges::all(centui::px(24));
    centui::alignment align_items = centui::alignment::end;
    centui::justification justify_content = centui::justification::center;
};

struct panel
{
    centui::length width = centui::px(300);
    centui::edges padding = centui::edges::all(centui::px(22));
    centui::length gap = centui::px(16);
    centui::states_color background_color = centui::states_color::all({24, 34, 48, 235});
};

struct heading
{
    float font_size = 26;
};

struct caption
{
    centui::states_color text_color = centui::states_color::all({170, 185, 204, 255});
};

struct control
{
    centui::length height = centui::px(44);
    centui::edges padding = centui::edges::all(centui::px(10));

    struct
    {
        centui::color normal{42, 57, 78, 255};
        centui::color hover{53, 77, 106, 255};
        centui::color pressed{31, 47, 66, 255};
        centui::color focused{53, 77, 106, 255};
    } background_color;
};

} // namespace cosinet