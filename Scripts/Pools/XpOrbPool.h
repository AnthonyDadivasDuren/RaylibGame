#pragma once

#include "raylib.h"
#include "Core/GameManager.h"
#include <vector>
#include <cmath>

struct XpOrb
{
    Vector2 position;
    Vector2 velocity;
    int xpValue;
    bool active;
    float radius;
    Color color;

    XpOrb() : position({ 0, 0 }), velocity({ 0, 0 }), xpValue(1), active(false), radius(4.0f), color(GREEN) {}

    void Activate(float x, float y, int value = 2)
    {
        position.x = x;
        position.y = y;
        velocity.x = 0;
        velocity.y = 0;
        xpValue = value;
        active = true;
        radius = 4.0f;
        Color greens[] = { GREEN, LIME, DARKGREEN };
        color = greens[GetRandomValue(0, 2)];
    }

    void Deactivate() { active = false; }

    void Update(float deltaTime, float playerX, float playerY)
    {
        if (!active) return;
        float dx = playerX - position.x;
        float dy = playerY - position.y;
        float dist = std::sqrt(dx * dx + dy * dy);
        const float baseMagnetRadius = 200.0f;
        float magnetRadius = baseMagnetRadius * GameManager::Instance().GetOrbMagnetismMultiplier();
        const float collectRadius = 45.0f;
        const float magnetSpeed = 280.0f;
        if (dist < collectRadius)
        {
            GameManager::Instance().AddXp(xpValue);
            Deactivate();
            return;
        }
        if (dist < magnetRadius && dist > 0.001f)
        {
            float nx = dx / dist;
            float ny = dy / dist;
            velocity.x = nx * magnetSpeed;
            velocity.y = ny * magnetSpeed;
        }
        position.x += velocity.x * deltaTime;
        position.y += velocity.y * deltaTime;
        velocity.x *= 0.92f;
        velocity.y *= 0.92f;
    }

    void Draw() const
    {
        if (!active) return;
        DrawCircleV(position, radius, color);
        DrawCircleLines((int)position.x, (int)position.y, radius, ColorBrightness(color, 0.4f));
    }
};

class XpOrbPool
{
    std::vector<XpOrb> pool;
    int poolSize;

public:
    XpOrbPool(int size) : poolSize(size)
    {
        pool.resize(size);
    }

    void Spawn(float x, float y)
    {
        float offsetX = (float)(GetRandomValue(-12, 12));
        float offsetY = (float)(GetRandomValue(-12, 12));
        for (int i = 0; i < poolSize; i++)
        {
            if (!pool[i].active)
            {
                pool[i].Activate(x + offsetX, y + offsetY);
                return;
            }
        }
    }

    void SpawnMany(float x, float y, int count)
    {
        for (int c = 0; c < count; c++)
        {
            float offsetX = (float)(GetRandomValue(-20, 20));
            float offsetY = (float)(GetRandomValue(-20, 20));
            for (int i = 0; i < poolSize; i++)
            {
                if (!pool[i].active)
                {
                    pool[i].Activate(x + offsetX, y + offsetY, 2);
                    break;
                }
            }
        }
    }

    void UpdateAll(float deltaTime, float playerX, float playerY)
    {
        for (int i = 0; i < poolSize; i++)
            pool[i].Update(deltaTime, playerX, playerY);
    }

    void DrawAll() const
    {
        for (int i = 0; i < poolSize; i++)
            pool[i].Draw();
    }

    void ResetPool()
    {
        for (int i = 0; i < poolSize; i++)
            pool[i].Deactivate();
    }

    int GetPoolSize() const { return poolSize; }
    XpOrb* GetOrb(int i) { return (i >= 0 && i < poolSize) ? &pool[i] : nullptr; }
};
