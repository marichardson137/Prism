#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "raylib.h"
#include "raymath.h"

void _CameraYaw(Camera* camera, float angle, bool rotateAroundTarget);
void _CameraPitch(Camera* camera, float angle, bool lockView, bool rotateAroundTarget, bool rotateUp);
void _CameraMoveToTarget(Camera* camera, float delta);

#endif