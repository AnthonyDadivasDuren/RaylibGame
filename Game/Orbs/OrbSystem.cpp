#include "OrbSystem.h"
#include "../../XpOrbPool.h"
#include "../../AmmoOrbPool.h"
#include "../../HealthOrbPool.h"

void OrbSystem::Update(float deltaTime, float playerX, float playerY,
                       XpOrbPool* xpPool, AmmoOrbPool* ammoPool, HealthOrbPool* healthPool)
{
    if (xpPool) xpPool->UpdateAll(deltaTime, playerX, playerY);
    if (ammoPool) ammoPool->UpdateAll(deltaTime, playerX, playerY);
    if (healthPool) healthPool->UpdateAll(deltaTime, playerX, playerY);
}

void OrbSystem::Reset(XpOrbPool* xpPool, AmmoOrbPool* ammoPool, HealthOrbPool* healthPool)
{
    if (xpPool) xpPool->ResetPool();
    if (ammoPool) ammoPool->ResetPool();
    if (healthPool) healthPool->ResetPool();
}

void OrbSystem::Draw(XpOrbPool* xpPool, AmmoOrbPool* ammoPool, HealthOrbPool* healthPool)
{
    if (xpPool) xpPool->DrawAll();
    if (ammoPool) ammoPool->DrawAll();
    if (healthPool) healthPool->DrawAll();
}
