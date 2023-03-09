#include <allegro5/base.h>
#include <allegro5/color.h>
#include <allegro5/allegro_primitives.h>
#include <utility>
#include <cmath>
#include <string>
#include "Collider.hpp"
#include "TrapDefense.hpp"
#include "AudioHelper.hpp"
#include "CannonBullet.hpp"
#include "Group.hpp"
#include "PlayScene.hpp"
#include "Point.hpp"
#include "Army.hpp"
#include <iostream>
//Defense(std::string imgDefense, float x, float y, float radius, float coolDown, int hp, int id, float shootRadius);
TrapDefense::TrapDefense(float x, float y) :
    Defense("play/trap.png", x, y, 40, 1, 50, 4, 40) {
    // Move center downward, since we the turret head is slightly biased upward.
    Anchor.y += 8.0f / GetBitmapHeight();
    show = false;
    start = true;
    span = 0;
}
void TrapDefense::trapon(Army* army) {
    if (army->speed > 5) army->speed -= 0.5;
    else   army->Hit(INFINITY);

    
    
}
void TrapDefense::Hit(float damage) {
    HP -= damage;
    if (HP <= 0) {
        //OnExplode();
        

        int x = static_cast<int>(floor(Position.x / PlayScene::BlockSize));
        int y = static_cast<int>(floor(Position.y / PlayScene::BlockSize));
        getPlayScene()->ClearMapState(x, y);
        getPlayScene()->TrapGroup->RemoveObject(objectIterator);
        //AudioHelper::PlayAudio("explosion.wav");
    }
}
void TrapDefense::Update(float deltaTime) {
    if (this->world) return;
    
    Sprite::Update(deltaTime);
    PlayScene* scene = getPlayScene();
    if (scene->world)return;
    if (!Enabled)
        return;
    for (auto& it : scene->ArmyGroup->GetObjects()) {
        Army* army = dynamic_cast<Army*>(it);
        if (!army->Visible)
            continue;
        if (Engine::Collider::IsCircleOverlap(Position, CollisionRadius, army->Position, army->CollisionRadius) && army->dio == false) {
            if (start) {
                AudioHelper::PlaySample("trap.mp3", false, 2.0);
                start = false;
            }
            show = true;
            trapon(army);
            return;
        }
    }
    if(show) span += 0.05;
    if (span >= 3)Hit(INFINITY);
    std::cout << span << "\n";

}
void TrapDefense::Draw() const {
    if (PlayScene::DebugMode || show) Sprite::Draw();
    if (PlayScene::DebugMode) {
        al_draw_circle(Position.x, Position.y, shootRadius, al_map_rgb(128, 128, 255), 2);
    }
}
void TrapDefense::CreateBullet(Engine::Point pt) {

}
