#pragma once

#include "../../ExpandingShockwave.h"
#include <vector>

class EnemyPool;
class ParticlePool;
class XpOrbPool;
class GameManager;


class ShockwaveSystem
{
public:
    ShockwaveSystem() = default;

    void AddShockwave(const ExpandingShockwave& sw);
    void Update(float deltaTime, EnemyPool* enemyPool, ParticlePool* particlePool, XpOrbPool* xpOrbPool, GameManager* gm);
    const std::vector<ExpandingShockwave>& GetShockwaves() const { return shockwaves_; }
    void Clear() { shockwaves_.clear(); }

    void Draw() const;

private:
    std::vector<ExpandingShockwave> shockwaves_;
};
