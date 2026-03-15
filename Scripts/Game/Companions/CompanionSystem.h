#pragma once

class EnemyPool;
class BulletPool;
class XpOrbPool;
class GameManager;

class CompanionSystem
{
public:
    static const int MaxCompanions = 2;

    CompanionSystem();

    void Update(float deltaTime, float playerX, float playerY, float playerLastMoveX, float playerLastMoveY,
                int screenWidth, int screenHeight,
                EnemyPool* enemyPool, BulletPool* bulletPool, XpOrbPool* xpOrbPool, GameManager* gm);

    float GetPistolX(int index) const;
    float GetPistolY(int index) const;
    float GetOrbX(int index) const;
    float GetOrbY(int index) const;
    float GetShotgunX(int index) const;
    float GetShotgunY(int index) const;

    void Draw() const;

    void Reset(float startX, float startY);

private:
    void ResolveOverlaps(GameManager* gm);

    float pistolX_[MaxCompanions];
    float pistolY_[MaxCompanions];
    float pistolFireCooldown_[MaxCompanions];
    float orbX_[MaxCompanions];
    float orbY_[MaxCompanions];
    float shotgunX_[MaxCompanions];
    float shotgunY_[MaxCompanions];
    float shotgunFireCooldown_[MaxCompanions];
};
