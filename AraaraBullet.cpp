#include <allegro5/base.h>
#include <random>
#include <string>
#include "Army.hpp"
#include "DirtyEffect.hpp"
#include "AraaraBullet.hpp"
#include "Group.hpp"
#include "PlayScene.hpp"
#include "Point.hpp"
#include "Collider.hpp"
class Defense;

//Bullet(std::string img, float speed, float damage, Engine::Point position, Engine::Point forwardDirection, float rotation, Defense* defenseParent);
AraaraBullet::AraaraBullet(Engine::Point position, Engine::Point forwardDirection, float rotation, Defense* parent) :
    Bullet("play/bullet-13.png", 1000, 2, position, forwardDirection, rotation, parent) {
}
void AraaraBullet::Update(float deltaTime) {
    
    Sprite::Update(deltaTime);
    PlayScene* scene = getPlayScene();
    if (scene->world)return;
    if (scene->coco) {
        getPlayScene()->BulletGroup->RemoveObject(objectIterator);
        return;
    }
    for (auto& it : scene->ArmyGroup->GetObjects()) {
        Army* army = dynamic_cast<Army*>(it);
        if (!army->Visible)
            continue;
        if (Engine::Collider::IsCircleOverlap(Position, CollisionRadius, army->Position, army->CollisionRadius&& army->dio == false)) {
            army->Hit(damage);
            if (army->speed > 20)army->speed -= 7;
            getPlayScene()->BulletGroup->RemoveObject(objectIterator);
            return;
        }
    }
    // Check if out of boundary.
    if (Position.x < 0 || Position.x > PlayScene::GetClientSize().x || Position.y < 0 || Position.y > PlayScene::GetClientSize().y)
        getPlayScene()->BulletGroup->RemoveObject(objectIterator);
}
