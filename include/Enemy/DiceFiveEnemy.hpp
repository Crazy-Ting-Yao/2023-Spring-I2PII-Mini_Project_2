#ifndef DICEFIVEENEMY_HPP
#define DICEFIVEENEMY_HPP
#include "Enemy.hpp"

class DiceFiveEnemy : public Enemy {
public:
    DiceFiveEnemy(int x, int y);
    void OnExplode() override;
};
#endif // DICEFIVEENEMY_HPP
