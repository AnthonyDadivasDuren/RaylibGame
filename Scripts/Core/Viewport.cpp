#include "Core/Viewport.h"
#include "raylib.h"

float Viewport::scale_ = 1.0f;
float Viewport::offsetX_ = 0.0f;
float Viewport::offsetY_ = 0.0f;

void Viewport::SetViewport(int windowWidth, int windowHeight)
{
    float scaleW = (float)windowWidth / (float)GameWidth;
    float scaleH = (float)windowHeight / (float)GameHeight;
    scale_ = (scaleW < scaleH) ? scaleW : scaleH;
    float drawW = (float)GameWidth * scale_;
    float drawH = (float)GameHeight * scale_;
    offsetX_ = ((float)windowWidth - drawW) * 0.5f;
    offsetY_ = ((float)windowHeight - drawH) * 0.5f;
}

float Viewport::GetGameMouseX()
{
    float screenX = (float)GetMouseX();
    return (screenX - offsetX_) / scale_;
}

float Viewport::GetGameMouseY()
{
    float screenY = (float)GetMouseY();
    return (screenY - offsetY_) / scale_;
}
