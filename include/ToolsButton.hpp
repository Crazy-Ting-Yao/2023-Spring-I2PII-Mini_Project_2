#ifndef TOOLSBUTTON_HPP
#define TOOLSBUTTON_HPP

#include <string>

#include "include/ImageButton.hpp"
#include "Sprite.hpp"

class PlayScene;

class ToolsButton : public Engine::ImageButton {
protected:
    PlayScene* getPlayScene();
public:
    Engine::Sprite pic;
    ToolsButton(std::string img, std::string imgIn , Engine::Sprite PIC, float x, float y);
    void Update(float deltaTime) override;
    void Draw() const override;
};


#endif //TOOLSBUTTON_HPP
