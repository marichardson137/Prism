#ifndef _SELECTION_H_
#define _SELECTION_H_

#include <vector>

#include "geometry.h"
#include "raylib.h"
#include "raymath.h"

typedef enum {
    MODEL = 0,
    POLYGON,
    VERTEX
} SelectionMode;

typedef enum {
    SELECT = 0,
    TRANSLATE,
    ROTATE,
    SCALE,
} EditMode;

typedef enum {
    NORMAL_AXIS = 0,
    X_AXIS,
    Y_AXIS,
    Z_AXIS,
} EditAxis;

const Vector3 X_AXIS_VECTOR = { 1.0f, 0.0f, 0.0f };
const Vector3 Y_AXIS_VECTOR = { 0.0f, 1.0f, 0.0f };
const Vector3 Z_AXIS_VECTOR = { 0.0f, 0.0f, 1.0f };

const float MOVE_SPEED = 0.02f;

class Selection {

public:
    SelectionMode selectionMode;
    EditMode editMode;
    EditAxis editAxis;
    vector<int> selectedPolygons;
    vector<int> selectedVertices;
    int activePolygon;
    int activeVertex;
    vector<Ray> rays;
    vector<prism::Model> editStack;

    Selection()
        : selectionMode(POLYGON)
        , editMode(SELECT)
        , editAxis(NORMAL_AXIS)
        , selectedPolygons()
        , selectedVertices()
        , activePolygon(-1)
        , activeVertex(-1)
        , rays()
        , editStack()
    {
    }

    void reset();
    void update(const Ray mouseRay, prism::Model& model);
    void drawRays();
    void saveModel(const prism::Model& model);
    void undo(prism::Model& model);
    void redo(prism::Model& model);

private:
    void changeSelectionMode();
    void changeEditMode(const prism::Model& model);
    void changeEditAxis();
    void select(const Ray mouseRay, prism::Model& model);
    void color(prism::Model& model);
    void edit(prism::Model& model);

    void EditPolygon(prism::Model& model, Vector3 axis);
    void EditVertex(prism::Model& model, Vector3 axis);
    void EditModel(prism::Model& model, Vector3 axis);
};

#endif
