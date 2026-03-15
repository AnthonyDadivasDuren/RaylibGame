#pragma once

class EnemyPool;

enum class EnemyType;

class EnemySpawner
{
public:
    EnemySpawner();

    void Update(float deltaTime, int wave, int screenWidth, int screenHeight, bool spawningEnabled, EnemyPool* enemyPool);

    void Reset();

private:
    float spawnTimer_;
};
