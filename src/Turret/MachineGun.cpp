#include <allegro5/base.h>
#include <cmath>
#include <string>

#include "include/AudioHelper.hpp"
#include "include/Bullet/FireBullet.hpp"
#include "include/Group.hpp"
#include "include/Turret/MachineGun.hpp"
#include "include/Scene/PlayScene.hpp"
#include "include/Point.hpp"
#include "include/Effect/ShootEffect.hpp"

const int MachineGun::Price = 50;
MachineGun::MachineGun(float x, float y) :
// TODO 3 (1/5): You can imitate the 2 files: 'PlugGunTurret.hpp', 'PlugGunTurret.cpp' to create a new turret.
        Turret("play/tower-base.png", "play/turret-1.png", x, y, 300, Price, 1.5, 1) {
    // Move center downward, since we the turret head is slightly biased upward
    Anchor.y += 8.0f / GetBitmapHeight();
}
void MachineGun::CreateBullet() {
    Engine::Point diff = Engine::Point(cos(Rotation - ALLEGRO_PI / 2), sin(Rotation - ALLEGRO_PI / 2));
    float rotation = atan2(diff.y, diff.x);
    Engine::Point normalized = diff.Normalize();
    // Change bullet position to the front of the gun barrel.
    getPlayScene()->BulletGroup->AddNewObject(new FireBullet(Position + normalized * 36, diff, rotation, this));
    // TODO 4 (2/2): Add a ShootEffect here. Remember you need to include the class.
    getPlayScene()->EffectGroup->AddNewObject(new ShootEffect(Position.x + normalized.x * 36 , Position.y + normalized.y * 36));
    AudioHelper::PlayAudio("gun.wav");
}
