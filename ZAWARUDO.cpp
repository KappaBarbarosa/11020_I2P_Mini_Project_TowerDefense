#include <allegro5/base.h>
#include <random>
#include <string>

#include "ShootEffect.hpp"
#include "DirtyEffect.hpp"
#include "ZAWARUDO.hpp"
#include "Group.hpp"
#include "PlayScene.hpp"
#include "Point.hpp"
#include "Defense.hpp"

class Defense;

//ArmyBullet::ArmyBullet(std::string img, float speed, float damage, Engine::Point position, Engine::Point forwardDirection, float rotation, Army* parent)
ZAWARUDO::ZAWARUDO(Engine::Point position, Engine::Point forwardDirection, float rotation, Army* parent) :
    ArmyBullet("play/the_world.png", 500, 5, position, forwardDirection, rotation - ALLEGRO_PI / 2, parent) {
}

void ZAWARUDO::OnExplode(Defense* defense) {
    getPlayScene()->EffectGroup->AddNewObject(new ShootEffect(defense->Position.x, defense->Position.y));
}

