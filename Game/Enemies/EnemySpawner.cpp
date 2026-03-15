#include "EnemySpawner.h"
#include "../../EnemyPool.h"
#include "../../EnemyFactory.h"
#include "raylib.h"
#include <cmath>

EnemySpawner::EnemySpawner() : spawnTimer_(0.0f)
{
}

void EnemySpawner::Update(float deltaTime, int wave, int screenWidth, int screenHeight, bool spawningEnabled, EnemyPool* enemyPool)
{
    float spawnIntervalThisWave = 3.0f;
    if (wave >= 2) spawnIntervalThisWave = 2.5f;
    if (wave >= 3) spawnIntervalThisWave = 2.0f;
    if (wave >= 4) spawnIntervalThisWave = 1.5f;
    if (wave >= 5) spawnIntervalThisWave = 1.0f;

    float spawnRateMult = 1.0f;
    if (wave >= 2 && wave <= 5) spawnRateMult = 1.0f + (wave - 1) * 0.25f;
    else if (wave >= 6 && wave <= 9) spawnRateMult = 2.0f + (wave - 5) * 0.50f;
    else if (wave >= 10) spawnRateMult = 4.0f + (wave - 9) * 1.0f;
    spawnRateMult *= 1.25f;
    if (wave <= 3) spawnRateMult *= 0.75f;
    spawnRateMult *= 0.75f;

    spawnIntervalThisWave /= spawnRateMult;
    if (spawnIntervalThisWave < 0.15f) spawnIntervalThisWave = 0.15f;

    spawnTimer_ += deltaTime;
    if (!spawningEnabled || spawnTimer_ < spawnIntervalThisWave || !enemyPool)
        return;

    spawnTimer_ = 0.0f;
    int count = (int)((float)GetRandomValue(2, 4) * spawnRateMult);
    if (count < 2) count = 2;

    for (int c = 0; c < count; c++)
    {
        float posX = 0.0f, posY = 0.0f;
        int edge = GetRandomValue(0, 3);
        if (edge == 0) { posX = (float)GetRandomValue(0, screenWidth); posY = -25.0f; }
        else if (edge == 1) { posX = (float)(screenWidth + 25); posY = (float)GetRandomValue(0, screenHeight); }
        else if (edge == 2) { posX = (float)GetRandomValue(0, screenWidth); posY = (float)(screenHeight + 25); }
        else { posX = -25.0f; posY = (float)GetRandomValue(0, screenHeight); }

        EnemyType type = EnemyType::GRUNT;
        if (wave >= 2)
        {
            int roll = GetRandomValue(1, 100);
            if (wave >= 3 && roll <= 6) type = EnemyType::SPLITTER;
            else if (wave >= 4 && roll >= 98) type = EnemyType::ARMORED;
            else if (wave >= 4 && roll > 50) type = EnemyType::SCOUT;
            else if (wave >= 3 && roll > 60) type = EnemyType::SCOUT;
            else if (roll > 80) type = EnemyType::SCOUT;
        }
        enemyPool->Spawn(type, posX, posY);
    }
}

void EnemySpawner::Reset()
{
    spawnTimer_ = 0.0f;
}
