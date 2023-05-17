#include <allegro5/allegro_primitives.h>
#include <allegro5/color.h>
#include <cmath>
#include <random>
#include <string>
#include <vector>

#include "include/Enemy/DiceSixEnemy.hpp"
#include "include/Effect/DirtyEffect.hpp"
#include "include/AudioHelper.hpp"
#include "include/Turret/Turret.hpp"
#include "include/Bullet/Bullet.hpp"
#include "include/Scene/PlayScene.hpp"
#include "include/Enemy/DiceFiveEnemy.hpp"
#include "include/Group.hpp"
#include "include/IObject.hpp"
#include "include/Effect/ExplosionEffect.hpp"
#include "include/LOG.hpp"
DiceSixEnemy::DiceSixEnemy(int x, int y) : Enemy("play/dice-6.png", x, y, 25, 10, 500, 100, 3) {

}
void DiceSixEnemy::OnExplode(){
    Enemy* enemy;
    getPlayScene()->EnemyGroup->AddNewObject(enemy = new DiceFiveEnemy(Position.x, Position.y));
    enemy->UpdatePath(getPlayScene()->mapDistance);
    // Compensate the time lost.
    enemy->Update(0);
    getPlayScene()->EnemyGroup->AddNewObject(enemy = new DiceFiveEnemy(Position.x, Position.y));
    enemy->UpdatePath(getPlayScene()->mapDistance);
    // Compensate the time lost.
    enemy->Update(0);
    getPlayScene()->EffectGroup->AddNewObject(new ExplosionEffect(Position.x, Position.y));
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> distId(1, 3);
    std::uniform_int_distribution<std::mt19937::result_type> dist(1, 20);
    for (int i = 0; i < 10; i++) {
        // Random add 10 dirty effects.
        getPlayScene()->GroundEffectGroup->AddNewObject(new DirtyEffect("play/dirty-" + std::to_string(distId(rng)) + ".png", dist(rng), Position.x, Position.y));
    }

}
