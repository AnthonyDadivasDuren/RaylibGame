#pragma once

/// Converts between window and fixed game coordinates (1800x1350).
/// Call SetViewport each frame with window size; use GetGameMouseX/Y for input.
class Viewport
{
public:
    static const int GameWidth = 1800;
    static const int GameHeight = 1350;

    static void SetViewport(int windowWidth, int windowHeight);
    static float GetGameMouseX();
    static float GetGameMouseY();
    static float GetScale() { return scale_; }
    static float GetOffsetX() { return offsetX_; }
    static float GetOffsetY() { return offsetY_; }

private:
    static float scale_;
    static float offsetX_;
    static float offsetY_;
};
