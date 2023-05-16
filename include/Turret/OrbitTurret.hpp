#ifndef ORBITTURRET_HPP
#define ORBITTURRET_HPP
#include "Turret.hpp"
#include "include/Bullet/Bullet.hpp"

class OrbitTurret : public Turret {
public:
    static const int Price;
    int orbitRadius = 200;
    OrbitTurret(float x, float y);
    void CreateBullet() override;
    void Update(float deltaTime) override;
    Bullet *bullets[4];
    void deletebullet(Bullet *bullet);
    ~OrbitTurret();
};
#endif // ORBITTURRET_HPP
