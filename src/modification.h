#ifndef _MODIFICATION_H_
#define _MODIFICATION_H_

#include "geometry.h"

typedef enum {
    DEFAULT = 0,
    TRANSFORM,
    SCALE,
    EXTRUDE,
    ROTATE
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

void EditPolygon(prism::Model& model, EditMode editMode, Vector3 axis, vector<int>& selectedPolygons, int& activePolygon);
void EditVertex(prism::Model& model, EditMode editMode, vector<int>& selectedVertices, int& activeVertex);

#endif
