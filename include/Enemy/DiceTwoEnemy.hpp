#ifndef DICETWOENEMY_HPP
#define DICETWOENEMY_HPP
#include "Enemy.hpp"

class DiceTwoEnemy : public Enemy {
public:
    DiceTwoEnemy(int x, int y);
    void OnExplode() override;
};
#endif // DICETWOENEMY_HPP
