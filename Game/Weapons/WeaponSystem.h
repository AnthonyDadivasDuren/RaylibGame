#pragma once

class EnemyPool;
class BulletPool;
class ParticlePool;
class GameManager;

class WeaponSystem
{
public:
    WeaponSystem();

    int GetWeapon() const { return currentWeapon_; }
    void SetWeapon(int w) { if (w == 0 || w == 1) currentWeapon_ = w; }
    float GetPistolFireInterval() const { return fireIntervalFollow_; }

    bool TryFirePistol(float deltaTime, float px, float py, float lastMoveX, float lastMoveY, EnemyPool* enemyPool, BulletPool* bulletPool, GameManager* gm,
                       bool bossActive = false, float bossX = 0.0f, float bossY = 0.0f);

    bool TryFireShotgun(float px, float py, BulletPool* bulletPool, ParticlePool* particlePool, GameManager* gm);

    bool TryFireAutoAttacker(float deltaTime, float px, float py, EnemyPool* enemyPool, BulletPool* bulletPool,
                             bool bossActive, float bossX, float bossY, GameManager* gm);

    void Reset();

private:
    int currentWeapon_;
    float fireCooldown_;
    float fireIntervalFollow_;
    float autoAttackerCooldown_;
};
