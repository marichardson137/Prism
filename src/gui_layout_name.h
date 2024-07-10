/*******************************************************************************************
 *
 *   LayoutName v1.0.0 - Tool Description
 *
 *   MODULE USAGE:
 *       #define GUI_LAYOUT_NAME_IMPLEMENTATION
 *       #include "gui_layout_name.h"
 *
 *       INIT: GuiLayoutNameState state = InitGuiLayoutName();
 *       DRAW: GuiLayoutName(&state);
 *
 *   LICENSE: Propietary License
 *
 *   Copyright (c) 2022 raylib technologies. All Rights Reserved.
 *
 *   Unauthorized copying of this file, via any medium is strictly prohibited
 *   This project is proprietary and confidential unless the owner allows
 *   usage in any other form by expresely written permission.
 *
 **********************************************************************************************/

#include "raylib.h"

// WARNING: raygui implementation is expected to be defined before including this header
#undef RAYGUI_IMPLEMENTATION
#include "raygui.h"

#include <string.h> // Required for: strcpy()

#ifndef GUI_LAYOUT_NAME_H
#define GUI_LAYOUT_NAME_H

typedef struct {
    bool DropdownBox009EditMode;
    int DropdownBox009Active;
    int ToggleGroup010Active;
    float ProgressBar012Value;

    // Custom state variables (depend on development software)
    // NOTE: This variables should be added manually if required

} GuiLayoutNameState;

#ifdef __cplusplus
extern "C" { // Prevents name mangling of functions
#endif

//----------------------------------------------------------------------------------
// Defines and Macros
//----------------------------------------------------------------------------------
//...

//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------
// ...

//----------------------------------------------------------------------------------
// Module Functions Declaration
//----------------------------------------------------------------------------------
GuiLayoutNameState InitGuiLayoutName(void);
void GuiLayoutName(GuiLayoutNameState* state);
static void Button001();
static void Button002();
static void Button003();
static void Button004();
static void Button005();
static void Button006();
static void Button007();
static void Button008();
static void Button011();

#ifdef __cplusplus
}
#endif

#endif // GUI_LAYOUT_NAME_H

/***********************************************************************************
 *
 *   GUI_LAYOUT_NAME IMPLEMENTATION
 *
 ************************************************************************************/
#if defined(GUI_LAYOUT_NAME_IMPLEMENTATION)

#include "raygui.h"

//----------------------------------------------------------------------------------
// Global Variables Definition
//----------------------------------------------------------------------------------
//...

//----------------------------------------------------------------------------------
// Internal Module Functions Definition
//----------------------------------------------------------------------------------
//...

//----------------------------------------------------------------------------------
// Module Functions Definition
//----------------------------------------------------------------------------------
GuiLayoutNameState InitGuiLayoutName(void)
{
    GuiLayoutNameState state = { 0 };

    state.DropdownBox009EditMode = false;
    state.DropdownBox009Active = 0;
    state.ToggleGroup010Active = 0;
    state.ProgressBar012Value = 0.0f;

    // Custom variables initialization

    return state;
}
static void Button001()
{
    // TODO: Implement control logic
}
static void Button002()
{
    // TODO: Implement control logic
}
static void Button003()
{
    // TODO: Implement control logic
}
static void Button004()
{
    // TODO: Implement control logic
}
static void Button005()
{
    // TODO: Implement control logic
}
static void Button006()
{
    // TODO: Implement control logic
}
static void Button007()
{
    // TODO: Implement control logic
}
static void Button008()
{
    // TODO: Implement control logic
}
static void Button011()
{
    // TODO: Implement control logic
}

void GuiLayoutName(GuiLayoutNameState* state)
{
    const char* Panel000Text = "";
    const char* Button001Text = "";
    const char* Button002Text = "";
    const char* Button003Text = "";
    const char* Button004Text = "";
    const char* Button005Text = "";
    const char* Button006Text = "";
    const char* Button007Text = "";
    const char* Button008Text = "";
    const char* DropdownBox009Text = "ONE;TWO;THREE";
    const char* ToggleGroup010Text = "ONE;TWO;THREE";
    const char* Button011Text = "SAMPLE TEXT";
    const char* ProgressBar012Text = "";

    if (state->DropdownBox009EditMode)
        GuiLock();

    GuiPanel((Rectangle) { 0, 0, 1104, 48 }, Panel000Text);
    if (GuiButton((Rectangle) { 8, 8, 32, 32 }, Button001Text))
        Button001();
    if (GuiButton((Rectangle) { 56, 8, 32, 32 }, Button002Text))
        Button002();
    if (GuiButton((Rectangle) { 104, 8, 32, 32 }, Button003Text))
        Button003();
    if (GuiButton((Rectangle) { 152, 8, 32, 32 }, Button004Text))
        Button004();
    if (GuiButton((Rectangle) { 440, 8, 32, 32 }, Button005Text))
        Button005();
    if (GuiButton((Rectangle) { 488, 8, 32, 32 }, Button006Text))
        Button006();
    if (GuiButton((Rectangle) { 536, 8, 32, 32 }, Button007Text))
        Button007();
    if (GuiButton((Rectangle) { 584, 8, 32, 32 }, Button008Text))
        Button008();
    GuiToggleGroup((Rectangle) { 840, 8, 40, 32 }, ToggleGroup010Text, &state->ToggleGroup010Active);
    if (GuiButton((Rectangle) { 8, 696, 120, 32 }, Button011Text))
        Button011();
    GuiProgressBar((Rectangle) { 8, 56, 120, 16 }, ProgressBar012Text, NULL, &state->ProgressBar012Value, 0, 1);
    if (GuiDropdownBox((Rectangle) { 976, 8, 120, 32 }, DropdownBox009Text, &state->DropdownBox009Active, state->DropdownBox009EditMode))
        state->DropdownBox009EditMode = !state->DropdownBox009EditMode;

    GuiUnlock();
}

#endif // GUI_LAYOUT_NAME_IMPLEMENTATION
