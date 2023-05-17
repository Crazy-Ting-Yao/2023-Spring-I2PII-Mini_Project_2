#ifndef DICETHREEENEMY_HPP
#define DICETHREEENEMY_HPP
#include "Enemy.hpp"

class DiceThreeEnemy : public Enemy {
public:
    DiceThreeEnemy(int x, int y);
    void OnExplode() override;
};
#endif // DICETHREEENEMY_HPP
