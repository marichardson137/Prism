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

void EditPolygon(prism::Model& model, EditMode editMode, vector<int>& selectedPolygons, int& activePolygon);
void EditVertex(prism::Model& model, EditMode editMode, vector<int>& selectedVertices, int& activeVertex);

#endif
