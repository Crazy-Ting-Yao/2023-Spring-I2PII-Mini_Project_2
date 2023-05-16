#ifndef SHOVEL_HPP
#define SHOVEL_HPP
#include "Sprite.hpp"
class Shovel : public Engine::Sprite{
public:
    Shovel(float x, float y);
    bool Enabled = true;
    bool Preview = false;
};




#endif //SHOVEL_HPP
