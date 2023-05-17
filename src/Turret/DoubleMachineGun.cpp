#include <allegro5/base.h>
#include <cmath>
#include <string>

#include "include/AudioHelper.hpp"
#include "include/Bullet/FireBullet.hpp"
#include "include/Group.hpp"
#include "include/Turret/DoubleMachineGun.hpp"
#include "include/Scene/PlayScene.hpp"
#include "include/Point.hpp"
#include "include/Effect/ShootEffect.hpp"

const int DoubleMachineGun::Price = 100;
DoubleMachineGun::DoubleMachineGun(float x, float y) :
        Turret("play/tower-base.png", "play/turret-2.png", x, y, 300, Price, 1.5, 9) {
    // Move center downward, since we the turret head is slightly biased upward
    Anchor.y += 8.0f / GetBitmapHeight();
}
void DoubleMachineGun::CreateBullet() {
    Engine::Point diff = Engine::Point(cos(Rotation - ALLEGRO_PI / 2), sin(Rotation - ALLEGRO_PI / 2));
    Engine::Point shift = Engine::Point(sin(Rotation - ALLEGRO_PI / 2),-cos(Rotation - ALLEGRO_PI / 2));
    float rotation = atan2(diff.y, diff.x);
    Engine::Point normalized = diff.Normalize();
    Engine::Point shiftnormalized = shift.Normalize();
    // Change bullet position to the front of the gun barrel.
    getPlayScene()->BulletGroup->AddNewObject(new FireBullet(Position + normalized * 36 + shiftnormalized * 6, diff, rotation, this));
    getPlayScene()->BulletGroup->AddNewObject(new FireBullet(Position + normalized * 36 - shiftnormalized * 6, diff, rotation, this));
    getPlayScene()->EffectGroup->AddNewObject(new ShootEffect(Position.x + normalized.x * 36 + shiftnormalized.x * 6, Position.y + normalized.y * 36 + shiftnormalized.y * 6));
    getPlayScene()->EffectGroup->AddNewObject(new ShootEffect(Position.x + normalized.x * 36 - shiftnormalized.x * 6, Position.y + normalized.y * 36 - shiftnormalized.y * 6));
    AudioHelper::PlayAudio("gun.wav");
}
