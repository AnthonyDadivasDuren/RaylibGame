#pragma once

#include <set>

struct ExpandingShockwave
{
    float x = 0.0f, y = 0.0f;
    float currentRadius = 0.0f;
    float maxRadius = 0.0f;
    float expansionSpeed = 0.0f;
    int excludeEnemyIndex = -1;
    std::set<int> hitIndices;
};
