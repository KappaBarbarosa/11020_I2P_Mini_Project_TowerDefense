#include <allegro5/base.h>
#include <cmath>
#include <iostream>
#include <string>

#include "Collider.hpp"
#include "AudioHelper.hpp"
#include "Group.hpp"
#include "PlayScene.hpp"
#include "Point.hpp"
#include "Army.hpp"
#include "Defense.hpp"
#include "WallDefense.hpp"
#include"Theworld.hpp"
#define TOP_LEFT 0
#define TOP_RIGHT 1
#define BOTTOM_LEFT 2
#define BOTTOM_RIGHT 3

#define LEFT 0
#define UP 1
#define RIGHT 2
#define DOWN 3


theworld::theworld(float x, float y) : Role("play/ice.png", x, y) {
    // Move center downward, since we the turret head is slightly biased upward.
    Anchor.y += 8.0f / GetBitmapHeight();
}

void theworld::THEWORLD(Defense* defense) {
    // Delete
//    getPlayScene()->EffectGroup->AddNewObject(new ShootEffect(defense->Position.x , defense->Position.y));
}
void theworld::timeup() {
    if (this != nullptr)
    {
        for (auto& it : lockedDefenses)
            if (it != nullptr)it->world = false;
        getPlayScene()->WorldGroup->RemoveObject(objectIterator);
    }
}
void theworld::Update(float deltaTime) {
    Sprite::Update(deltaTime);
    PlayScene* scene = getPlayScene();
    // Can be improved by Spatial Hash, Quad Tree, ...
    // However simply loop through all enemies is enough for this program.
    for (auto& it : scene->DefenseGroup->GetObjects()) {
        Defense* defense = dynamic_cast<Defense*>(it);
        if (Engine::Collider::IsCircleOverlap(Position, CollisionRadius, defense->Position, defense->CollisionRadius)) {
            lockedDefenses.push_back(defense);
            //THEWORLD(defense);
            defense->world = true;
        }
    }

    // Check if out of boundary.
    if (Position.x < 0 || Position.x > PlayScene::GetClientSize().x || Position.y < 0 || Position.y > PlayScene::GetClientSize().y)
        getPlayScene()->BulletGroup->RemoveObject(objectIterator);
}