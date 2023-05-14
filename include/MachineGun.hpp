#ifndef MACHINEGUN_HPP
#define MACHINEGUN_HPP
#include "Turret.hpp"

class MachineGun : public Turret {
public:
    static const int Price;
    MachineGun(float x, float y);
    void CreateBullet() override;
};
#endif // MACHINEGUN_HPP
