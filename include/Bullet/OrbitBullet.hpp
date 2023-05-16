#ifndef ORBITBULLET_HPP
#define ORBITBULLET_HPP
#include "include/Bullet/Bullet.hpp"
#include "include/Turret/OrbitTurret.hpp"
class Enemy;
class Turret;
namespace Engine {
    struct Point;
}  // namespace Engine

class OrbitBullet : public Bullet {
public:
    explicit OrbitBullet(Engine::Point position, Engine::Point forwardDirection, float rotation, OrbitTurret* parent, double theta, int radius);
    void OnExplode(Enemy* enemy) override;
    void Update(float deltaTime) override;
    int radius;
    double theta;
    double omega = ALLEGRO_PI/2;
    OrbitTurret* Orbitparent;
};
#endif // ORBITBULLET_HPP
