#ifndef ORBITBULLET_HPP
#define ORBITBULLET_HPP
#include "include/Bullet/Bullet.hpp"

class Enemy;
class Turret;
namespace Engine {
    struct Point;
}  // namespace Engine

class OrbitBullet : public Bullet {
public:
    explicit OrbitBullet(Engine::Point position, Engine::Point forwardDirection, float rotation, Turret* parent, double theta, int radius);
    void OnExplode(Enemy* enemy) override;
    void Update(float deltaTime) override;
    int radius;
    double theta;
    double omega = ALLEGRO_PI/2;
};
#endif // ORBITBULLET_HPP
