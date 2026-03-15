#pragma once

class EnemyPool;
class BulletPool;
class GameManager;
class Player;

class EnemySystem
{
public:
    EnemySystem() = default;

    void Update(float deltaTime, float playerX, float playerY, int wave, EnemyPool* enemyPool);
    void HandlePlayerCollisions(Player* player, EnemyPool* enemyPool, BulletPool* bulletPool, GameManager* gm, float playerX, float playerY);
};
