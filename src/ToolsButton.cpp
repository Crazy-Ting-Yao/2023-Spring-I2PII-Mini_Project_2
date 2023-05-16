#include <allegro5/color.h>

#include "include/GameEngine.hpp"
#include "include/Scene/IScene.hpp"
#include "include/Scene/PlayScene.hpp"
#include "include/ToolsButton.hpp"

PlayScene* ToolsButton::getPlayScene() {
    return dynamic_cast<PlayScene*>(Engine::GameEngine::GetInstance().GetActiveScene());
}
ToolsButton::ToolsButton(std::string img, std::string imgIn, Engine::Sprite PIC, float x, float y) :
    ImageButton(img, imgIn, x, y), pic(PIC) {
}
void ToolsButton::Update(float deltaTime) {
    ImageButton::Update(deltaTime);
    Enabled = true;
    pic.Tint = al_map_rgba(255, 255, 255, 255);
}
void ToolsButton::Draw() const {
    ImageButton::Draw();
    pic.Draw();
}
