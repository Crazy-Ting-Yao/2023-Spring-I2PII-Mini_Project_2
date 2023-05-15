#include <allegro5/base.h>
#include <cmath>
#include <string>

#include "include/AudioHelper.hpp"
#include "include/Bullet/OrbitBullet.hpp"
#include "include/Group.hpp"
#include "include/Turret/OrbitTurret.hpp"
#include "include/Scene/PlayScene.hpp"
#include "include/Point.hpp"

#include "include/Effect/ShootEffect.hpp"

const int OrbitTurret::Price = 50;
OrbitTurret::OrbitTurret(float x, float y) :
    Turret("play/tower-base.png", "play/turret-4.png", x, y, 200, Price, 1.5, 1) {
}
void OrbitTurret::CreateBullet() {
    Engine::Point shift1(orbitRadius, 0);
    Engine::Point shift2(0, orbitRadius);

    getPlayScene()->BulletGroup->AddNewObject(new OrbitBullet(Position + shift1,  shift2,ALLEGRO_PI / 2, this, 0, orbitRadius));
    getPlayScene()->BulletGroup->AddNewObject(new OrbitBullet(Position + shift2, shift1, ALLEGRO_PI / 2, this, ALLEGRO_PI/2, orbitRadius));
    getPlayScene()->BulletGroup->AddNewObject(new OrbitBullet(Position - shift1, shift2 ,ALLEGRO_PI / 2, this, ALLEGRO_PI, orbitRadius));
    getPlayScene()->BulletGroup->AddNewObject(new OrbitBullet(Position - shift2, shift1, ALLEGRO_PI / 2, this, ALLEGRO_PI*3/2, orbitRadius));
    reload = 4;
}

void OrbitTurret::Update(float deltaTime){
    Sprite::Update(deltaTime);
    imgBase.Position = Position;
    imgBase.Tint = Tint;
    if (!Enabled)
        return;
    if(reload <= 0) CreateBullet();
}
