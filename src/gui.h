#ifndef RAYGUI_IMPLEMENTATION
#include "raygui.h"
#endif

#ifndef _GUI_H_
#define _GUI_H_

#include "geometry.h"
#include "state.h"

class Layout {
public:
    Layout()
        : RenderModeTG(0)
    {
    }

    void render(float screenWidth, float screenHeight, State& state)
    {
        Vector2 screenCenter = { screenWidth / 2.0f, screenHeight / 2.0f };

        // =================== Left ===================
        Rectangle ImportButton = { 8, 64, 32, 32 };
        if (GuiButton(ImportButton, GuiIconText(ICON_FILE_OPEN, nullptr)))
            ImportModel();

        Rectangle ExportButton = { 8, 104, 32, 32 };
        if (GuiButton(ExportButton, GuiIconText(ICON_FILE_SAVE, nullptr)))
            ExportModel(state.model);

        Rectangle CubeResetButton = { 8, 144, 32, 32 };
        if (GuiButton(CubeResetButton, GuiIconText(ICON_PLAYER_STOP, nullptr)))
            CubeReset(state.model);

        Rectangle CylinderResetButton = { 8, 184, 32, 32 };
        if (GuiButton(CylinderResetButton, GuiIconText(ICON_PLAYER_RECORD, nullptr)))
            CylinderReset(state.model);

        // =================== Top ===================
        Rectangle UndoButton = { screenCenter.x - 148, 8, 32, 32 };
        if (GuiButton(UndoButton, GuiIconText(ICON_UNDO, nullptr)))
            Undo(state.selection, state.model);

        Rectangle RenderModeButtons = { screenCenter.x - 108, 8, 72, 32 };
        GuiToggleGroup(RenderModeButtons, "DEFAULT;WIREFRAME;SHADED", &RenderModeTG);

        Rectangle RedoButton = { screenCenter.x + 120, 8, 32, 32 };
        if (GuiButton(RedoButton, GuiIconText(ICON_REDO, nullptr)))
            Redo(state.selection, state.model);

        // =================== Right ===================
        Rectangle CenterCameraButton = { screenWidth - 40, 64, 32, 32 };
        if (GuiButton(CenterCameraButton, GuiIconText(ICON_CAMERA, nullptr)))
            CenterCamera(state.camera);

        Rectangle CenterModelButton = { screenWidth - 40, 104, 32, 32 };
        if (GuiButton(CenterModelButton, GuiIconText(ICON_ZOOM_MEDIUM, nullptr)))
            CenterModel(state.model);

        // =================== Bottom ===================
    }

    void ImportModel()
    {
    }

    void ExportModel(const prism::Model& model)
    {
        model.exportSTL("model.stl");
    }

    void CubeReset(prism::Model& model)
    {
        model = prism::Model(prism::CUBE);
    }

    void CylinderReset(prism::Model& model)
    {
        model = prism::Model(prism::CYLINDER);
    }

    void Undo(Selection& selection, prism::Model& model)
    {
        selection.undo(model);
    }

    void Redo(Selection& selection, prism::Model& model)
    {
        ;
    }

    void CenterCamera(Camera& camera)
    {
        camera.position = (Vector3) { 10.0f, 10.0f, 10.0f };
        camera.target = (Vector3) { 0.0f, 0.5f, 0.0f };
        camera.up = (Vector3) { 0.0f, 1.0f, 0.0f };
        camera.fovy = 45.0f;
        ;
    }

    void CenterModel(prism::Model& model)
    {
        Vector3 modelCenter = model.computeCenter();
        for (prism::Vertex& vertex : model.vertices) {
            vertex = Vector3Subtract(vertex, modelCenter);
        }
    }

    // =============== GUI State ===============
    int RenderModeTG;
};

#endif
