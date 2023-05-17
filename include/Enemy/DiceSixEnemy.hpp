#ifndef DICESIXENEMY_HPP
#define DICESIXENEMY_HPP
#include "Enemy.hpp"

class DiceSixEnemy : public Enemy {
public:
    DiceSixEnemy(int x, int y);
    void OnExplode() override;
};
#endif // DICESIXENEMY_HPP
