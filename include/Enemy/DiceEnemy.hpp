#ifndef DICEENEMY_HPP
#define DICEENEMY_HPP
#include "Enemy.hpp"

class DiceEnemy : public Enemy {
public:
    DiceEnemy(int x, int y, bool invincible = false, float invincibleTime = 0);
};
#endif // DICEENEMY_HPP
