#ifndef _SELECTION_H_
#define _SELECTION_H_

#include <vector>

#include "geometry.h"
#include "raylib.h"
#include "raymath.h"
#include "modification.h"

typedef enum {
    MODEL = 0,
    POLYGON,
    VERTEX
} SelectionMode;

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

    Selection()
        : selectionMode(POLYGON)
        , editMode(SELECT)
        , editAxis(NORMAL_AXIS)
        , selectedPolygons()
        , selectedVertices()
        , activePolygon(-1)
        , activeVertex(-1)
        , rays()
    {
    }

    void reset();
    void update(const Ray mouseRay, prism::Model& model);
    void drawRays();

private:
    void changeSelectionMode();
    void changeEditMode();
    void changeEditAxis();
    void select(const Ray mouseRay, prism::Model& model);
    void color(prism::Model& model);
    void edit(prism::Model& model);
};

#endif
