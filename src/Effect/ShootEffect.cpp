#include <cmath>
#include <string>

#include "include/Effect/ShootEffect.hpp"
#include "include/GameEngine.hpp"
#include "include/Group.hpp"
#include "include/Scene/IScene.hpp"
#include "include/Scene/PlayScene.hpp"
#include "include/Resources.hpp"

// TODO 4 (1/2): You can imitate the 2 files: 'ExplosionEffect.hpp', 'ExplosionEffect.cpp' to create shoot effect.
PlayScene* ShootEffect::getPlayScene() {
    return dynamic_cast<PlayScene*>(Engine::GameEngine::GetInstance().GetActiveScene());
}
ShootEffect::ShootEffect(float x, float y) : Sprite("play/shoot-1.png", x, y), timeTicks(0) {
    for (int i = 1; i <= 4; i++) {
        bmps.push_back(Engine::Resources::GetInstance().GetBitmap("play/shoot-" + std::to_string(i) + ".png"));
    }
}
void ShootEffect::Update(float deltaTime) {
    timeTicks += deltaTime;
    if (timeTicks >= timeSpan) {
        getPlayScene()->EffectGroup->RemoveObject(objectIterator);
        return;
    }
    int phase = floor(timeTicks / timeSpan * bmps.size());
    bmp = bmps[phase];
    Sprite::Update(deltaTime);
}
