#include <allegro5/base.h>
#include <random>
#include <string>
#include <iostream>
#include "include/Effect/DirtyEffect.hpp"
#include "include/Enemy/Enemy.hpp"
#include "include/Bullet/OrbitBullet.hpp"
#include "include/Group.hpp"
#include "include/Scene/PlayScene.hpp"
#include "include/Point.hpp"
#include "include/Turret/Turret.hpp"
#include "include/Collider.hpp"
class Turret;

OrbitBullet::OrbitBullet(Engine::Point position, Engine::Point forwardDirection, float rotation, OrbitTurret* parent , double theta, int radius) :
    Bullet("play/bullet-8.png", 300, 2, position, forwardDirection, rotation, parent) , theta(theta), radius(radius), Orbitparent(parent) {
}

void OrbitBullet::OnExplode(Enemy* enemy) {
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist(2, 5);
    getPlayScene()->GroundEffectGroup->AddNewObject(new DirtyEffect("play/dirty-1.png", dist(rng), enemy->Position.x, enemy->Position.y));
    parent->reload-=1;
}

void OrbitBullet::Update(float deltaTime){
    theta = theta + omega * deltaTime;
    if(theta > 2 * ALLEGRO_PI) theta -= 2 * ALLEGRO_PI;
    Position.x = parent->Position.x + radius * cos(theta);
    Position.y = parent->Position.y + radius * sin(theta);
    PlayScene* scene = getPlayScene();
    // Can be improved by Spatial Hash, Quad Tree, ...
    // However simply loop through all enemies is enough for this program.
    for (auto& it : scene->EnemyGroup->GetObjects()) {
        Enemy* enemy = dynamic_cast<Enemy*>(it);
        if (!enemy->Visible)
            continue;
        if (Engine::Collider::IsCircleOverlap(Position, CollisionRadius, enemy->Position, enemy->CollisionRadius)) {
            OnExplode(enemy);
            enemy->Hit(damage);
            Orbitparent->deletebullet(this);
            getPlayScene()->BulletGroup->RemoveObject(objectIterator);
            return;
        }
    }
}