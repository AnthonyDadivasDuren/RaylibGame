#pragma once

class EnemyPool;
class BulletPool;
class GameManager;

namespace Pistol
{
    void Fire(float px, float py, float lastMoveX, float lastMoveY, EnemyPool* enemyPool, BulletPool* bulletPool, GameManager* gm,
              bool bossActive = false, float bossX = 0.0f, float bossY = 0.0f);
}
