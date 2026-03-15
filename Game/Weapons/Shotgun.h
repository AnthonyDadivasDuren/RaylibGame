#pragma once

class BulletPool;
class ParticlePool;
class GameManager;

namespace Shotgun
{
    void Fire(float px, float py, BulletPool* bulletPool, ParticlePool* particlePool, GameManager* gm);
}
