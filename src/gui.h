#ifndef RAYGUI_IMPLEMENTATION
#include "raygui.h"
#endif

#ifndef _GUI_H_
#define _GUI_H_

class Layout {
public:
    Layout()
        : RenderModeTG(0)
    {
    }

    void draw()
    {
        if (GuiButton((Rectangle) { 8, 50, 32, 32 }, GuiIconText(ICON_FILE_OPEN, nullptr)))
            Button001();
        if (GuiButton((Rectangle) { 8, 90, 32, 32 }, GuiIconText(ICON_FILE_SAVE, nullptr)))
            Button002();
        GuiToggleGroup((Rectangle) { 840, 8, 75, 32 }, "DEFAULT;WIREFRAME;SHADED", &RenderModeTG);
    }

    void Button001()
    {
    }

    void Button002()
    {
    }

    int RenderModeTG;
};

#endif
