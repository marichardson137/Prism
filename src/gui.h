#ifndef RAYGUI_IMPLEMENTATION
#include "raygui.h"
#endif

#ifndef _GUI_H_
#define _GUI_H_

#include "geometry.h"
#include "state.h"

#define DEFAULT_DIMENSION 64

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
        Rectangle ImportButton = { DEFAULT_DIMENSION / 8, DEFAULT_DIMENSION * 2, DEFAULT_DIMENSION, DEFAULT_DIMENSION };
        if (GuiButton(ImportButton, GuiIconText(ICON_FILE_OPEN, nullptr)))
            ImportModel();

        Rectangle ExportButton = { DEFAULT_DIMENSION / 8, DEFAULT_DIMENSION * 3.25, DEFAULT_DIMENSION, DEFAULT_DIMENSION };
        if (GuiButton(ExportButton, GuiIconText(ICON_FILE_SAVE, nullptr)))
            ExportModel(state.model);

        Rectangle CubeResetButton = { DEFAULT_DIMENSION / 8, DEFAULT_DIMENSION * 4.5, DEFAULT_DIMENSION, DEFAULT_DIMENSION };
        if (GuiButton(CubeResetButton, GuiIconText(ICON_PLAYER_STOP, nullptr)))
            CubeReset(state.model);

        Rectangle CylinderResetButton = { DEFAULT_DIMENSION / 8, DEFAULT_DIMENSION * 5.75, DEFAULT_DIMENSION, DEFAULT_DIMENSION };
        if (GuiButton(CylinderResetButton, GuiIconText(ICON_PLAYER_RECORD, nullptr)))
            CylinderReset(state.model);

        // =================== Top ===================
        Rectangle UndoButton = { screenCenter.x - DEFAULT_DIMENSION * 4.625, DEFAULT_DIMENSION / 8, DEFAULT_DIMENSION, DEFAULT_DIMENSION };
        if (GuiButton(UndoButton, GuiIconText(ICON_UNDO, nullptr)))
            Undo(state.selection, state.model);

        Rectangle RenderModeButtons = { screenCenter.x - DEFAULT_DIMENSION * 3.375, DEFAULT_DIMENSION / 8, DEFAULT_DIMENSION * 2.25, DEFAULT_DIMENSION };
        GuiToggleGroup(RenderModeButtons, "DEFAULT;WIREFRAME;SHADED", &RenderModeTG);

        Rectangle RedoButton = { screenCenter.x + DEFAULT_DIMENSION * 3.75, DEFAULT_DIMENSION / 8, DEFAULT_DIMENSION, DEFAULT_DIMENSION };
        if (GuiButton(RedoButton, GuiIconText(ICON_REDO, nullptr)))
            Redo(state.selection, state.model);

        // =================== Right ===================
        Rectangle CenterCameraButton = { screenWidth - DEFAULT_DIMENSION * 1.25, DEFAULT_DIMENSION * 2, DEFAULT_DIMENSION, DEFAULT_DIMENSION };
        if (GuiButton(CenterCameraButton, GuiIconText(ICON_CAMERA, nullptr)))
            CenterCamera(state.camera);

        Rectangle CenterModelButton = { screenWidth - DEFAULT_DIMENSION * 1.25, DEFAULT_DIMENSION * 3.25, DEFAULT_DIMENSION, DEFAULT_DIMENSION };
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
