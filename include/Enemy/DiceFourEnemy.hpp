#ifndef DICEFOURENEMY_HPP
#define DICEFOURENEMY_HPP
#include "Enemy.hpp"

class DiceFourEnemy : public Enemy {
public:
    DiceFourEnemy(int x, int y);
    void OnExplode() override;
};
#endif // DICEFOURENEMY_HPP
