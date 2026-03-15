#pragma once

class XpOrbPool;
class AmmoOrbPool;
class HealthOrbPool;

class OrbSystem
{
public:
    OrbSystem() = default;

    void Update(float deltaTime, float playerX, float playerY,
                XpOrbPool* xpPool, AmmoOrbPool* ammoPool, HealthOrbPool* healthPool);

    void Reset(XpOrbPool* xpPool, AmmoOrbPool* ammoPool, HealthOrbPool* healthPool);

    void Draw(XpOrbPool* xpPool, AmmoOrbPool* ammoPool, HealthOrbPool* healthPool);
};
