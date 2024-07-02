#include "rendering.h"

void DrawText3D(Camera camera, Vector3 position, const char* text, int fontSize, Color color)
{
    // Convert the 3D position to 2D screen space
    Vector2 screenPos = GetWorldToScreen(position, camera);
    // Draw the text at the 2D screen position
    DrawText(text, (int)screenPos.x, (int)screenPos.y, fontSize, color);
}