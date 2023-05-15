#ifndef DOUBLEMACHINEGUN_HPP
#define DOUBLEMACHINEGUN_HPP
#include "Turret.hpp"

class DoubleMachineGun : public Turret {
public:
    static const int Price;
    DoubleMachineGun(float x, float y);
    void CreateBullet() override;
};
#endif // DOUBLEMACHINEGUN_HPP
