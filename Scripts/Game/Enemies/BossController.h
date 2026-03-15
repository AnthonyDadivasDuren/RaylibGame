#pragma once

class GameManager;
class Player;
class BulletPool;
class ParticlePool;
class EnemyPool;
class XpOrbPool;

class BossController
{
public:
    static const int ExplosionProjectileCount = 24;
    static const int ExplosionDamage = 14;

    BossController();

    void Spawn(float centerX, float centerY);
    bool IsActive() const { return active_; }
    float GetX() const { return x_; }
    float GetY() const { return y_; }
    float GetRadius() const { return radius_; }
    int GetHealth() const { return health_; }
    int GetMaxHealth() const { return maxHealth_; }
    float GetExplosionFlashTimer() const { return explosionFlashTimer_; }
    float GetHitFlashTimer() const { return hitFlashTimer_; }

    void Update(float deltaTime, float playerX, float playerY, float playerSize, bool playerVulnerable,
                float playerLastMoveX, float playerLastMoveY, float playerSpeedMult,
                int screenWidth, int screenHeight,
                GameManager* gm, Player* player, BulletPool* bulletPool, ParticlePool* particlePool, EnemyPool* enemyPool);

    bool TakeDamage(int amount);
    void PlayDeathEffect(BulletPool* bulletPool, ParticlePool* particlePool);

    void CheckBulletHits(BulletPool& bulletPool, ParticlePool* particlePool, GameManager* gm);

    void TickExplosionFlashTimer(float deltaTime);

    void Draw(int screenWidth, int screenHeight) const;


    void OnWave10WaveComplete(EnemyPool* enemyPool, XpOrbPool* xpOrbPool, GameManager* gm);
    void UpdateWave10Transition(float deltaTime, EnemyPool* enemyPool, XpOrbPool* xpOrbPool, GameManager* gm, float spawnCenterX, float spawnCenterY);
    bool IsBossSpawnTriggered() const { return bossSpawnTriggered_; }
    float GetWave10FlashTimer() const { return wave10FlashTimer_; }
    void ResetWave10State();

private:
    bool active_;
    float x_, y_;
    int health_, maxHealth_;
    float radius_;
    int state_;         
    float stateTimer_;
    float dashTimer_;
    float tellDirX_, tellDirY_;
    float dashDirX_, dashDirY_;
    float dashTargetX_, dashTargetY_;
    float tellDuration_;
    float dashDuration_;
    float dashSpeed_;
    float tellBackDist_;
    float explosionSpeed_;
    float explosionFlashTimer_;
    float hitFlashTimer_;
    float idleMoveSpeed_;
    float dashDamageToPlayer_;
    bool bossSpawnTriggered_;
    float bossSpawnDelayTimer_;
    float wave10FlashTimer_;
};
