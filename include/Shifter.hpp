#ifndef SHIFTER_HPP
#define SHIFTER_HPP
#include "Sprite.hpp"
class Shifter : public Engine::Sprite{
public:
    Shifter(float x, float y);
    bool Enabled = true;
    bool Preview = false;
};




#endif //SHIFTER_HPP
