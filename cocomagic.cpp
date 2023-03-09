#include <allegro5/base.h>
#include <cmath>
#include <iostream>
#include <string>
#include <allegro5/allegro_primitives.h>
#include <allegro5/color.h>
#include "Collider.hpp"
#include "AudioHelper.hpp"
#include "Group.hpp"
#include "PlayScene.hpp"
#include "Point.hpp"
#include "Army.hpp"
#include "Defense.hpp"
#include "WallDefense.hpp"
#include"cocomagic.hpp"
#include "Resources.hpp"
#define TOP_LEFT 0
#define TOP_RIGHT 1
#define BOTTOM_LEFT 2
#define BOTTOM_RIGHT 3

#define LEFT 0
#define UP 1
#define RIGHT 2
#define DOWN 3


cocomagic::cocomagic(Cococute* coco) : Role("play/pixlr-bg-result(0).png", coco->Position.x, coco->Position.y+28) {
    // Move center downward, since we the turret head is slightly biased upward.
    Anchor.y += 8.0f / GetBitmapHeight();
    Coco = coco;
    for (int i = 0; i <= 49; i++) {
        bmps.push_back(Engine::Resources::GetInstance().GetBitmap("play/pixlr-bg-result(" + std::to_string(i) + ").png"));
    }
    over = false;
}


void cocomagic::Update(float deltaTime) {
    Sprite::Update(deltaTime);
    timeTicks += deltaTime;
    if (timeTicks >= timeSpan) {
        timeTicks = 0;
    }
    int phase = floor(timeTicks / timeSpan * bmps.size());
    bmp = bmps[phase];
    PlayScene* scene = getPlayScene();
    // Can be improved by Spatial Hash, Quad Tree, ...
    // However simply loop through all enemies is enough for this program.
    for (auto& it : scene->ArmyGroup->GetObjects()) {
        Army* army = dynamic_cast<Army*>(it);
        if (Engine::Collider::IsCircleOverlap(Coco->Position, 100, army->Position, army->CollisionRadius)) {
            army->Health(0.3);
            //getPlayScene()->WorldGroup->RemoveObject(objectIterator);
        }
    }
    Position.x = Coco->Position.x ;
    Position.y = Coco->Position.y + 28;
    gone();
}
void cocomagic::Draw()const {
    {
        if (PlayScene::DebugMode) {
            // Draw collision radius.
            al_draw_circle(Coco->Position.x, Coco->Position.y, 150, al_map_rgb(255, 0, 0), 2);
        }
        al_draw_tinted_scaled_rotated_bitmap(bmp.get(), Tint, Anchor.x * GetBitmapWidth(), Anchor.y * GetBitmapHeight(),
            Position.x, Position.y, 5*Size.x / GetBitmapWidth(), 5*Size.y / GetBitmapHeight(), Rotation, 0);
    }
}