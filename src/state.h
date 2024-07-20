#ifndef _STATE_H_
#define _STATE_H_

#include "geometry.h"
#include "selection.h"

#include "raylib.h"
#include "raymath.h"

typedef struct {
    prism::Model model; // TODO:: Convert to vector
    Selection selection;
    Camera camera;
    Vector2 mousePos;
    Ray mouseRay;
} State;

#endif