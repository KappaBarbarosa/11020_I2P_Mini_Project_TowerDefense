#include <allegro5/base.h>
#include <random>
#include <string>

#include "ShootEffect.hpp"
#include "DirtyEffect.hpp"
#include "Cocobullet.hpp"
#include "Group.hpp"
#include "PlayScene.hpp"
#include "Point.hpp"
#include "Defense.hpp"
#include "Collider.hpp"
class Defense;

//ArmyBullet::ArmyBullet(std::string img, float speed, float damage, Engine::Point position, Engine::Point forwardDirection, float rotation, Army* parent)
Cocobullet::Cocobullet(Engine::Point position, Engine::Point forwardDirection, float rotation, Army* parent) :
    ArmyBullet("play/cocoweapon.png", 100, 10, position, forwardDirection, rotation - ALLEGRO_PI / 2, parent) {
}

void Cocobullet::OnExplode(Defense* defense) {
    getPlayScene()->EffectGroup->AddNewObject(new ShootEffect(defense->Position.x, defense->Position.y));
}
void Cocobullet::Update(float deltaTime) {
    Sprite::Update(deltaTime);
    PlayScene* scene = getPlayScene();
    //if (scene->world)return;
    if (scene->coco) {
        getPlayScene()->BulletGroup->RemoveObject(objectIterator);
        return;
    }
    // Can be improved by Spatial Hash, Quad Tree, ...
    // However simply loop through all enemies is enough for this program.
    for (auto& it : scene->DefenseGroup->GetObjects()) {
        Defense* defense = dynamic_cast<Defense*>(it);
        if (Engine::Collider::IsCircleOverlap(Position, CollisionRadius, defense->Position, defense->CollisionRadius)) {
            OnExplode(defense);
            defense->Hit(0.1);
            defense->confuse();
            getPlayScene()->BulletGroup->RemoveObject(objectIterator);
            return;
        }
    }
    for (auto& it : scene->WallGroup->GetObjects()) {
        Defense* defense = dynamic_cast<Defense*>(it);
        if (Engine::Collider::IsCircleOverlap(Position, CollisionRadius, defense->Position, defense->CollisionRadius)) {
            OnExplode(defense);
            defense->Hit(50);
            getPlayScene()->BulletGroup->RemoveObject(objectIterator);
            return;
        }
    }

    // Check if out of boundary.
    if (Position.x < 0 || Position.x > PlayScene::GetClientSize().x || Position.y < 0 || Position.y > PlayScene::GetClientSize().y)
        getPlayScene()->BulletGroup->RemoveObject(objectIterator);
}

