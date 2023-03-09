#include <allegro5/color.h>
#include <allegro5/allegro_primitives.h>
#include <cmath>
#include <utility>
#include "Collider.hpp"
#include "AudioHelper.hpp"
#include "Army.hpp"
#include "GameEngine.hpp"
#include "Group.hpp"
#include "IObject.hpp"
#include "IScene.hpp"
#include "PlayScene.hpp"
#include "Point.hpp"
#include <iostream>
///#include "Star_Platinum.hpp"
#include "Jotaro.hpp"
#include "Star_platinum.hpp"
//float x, float y, float radius, float coolDown, int hp, int id, float shootRadius
Jotaro::Jotaro(float x, float y) :
    Defense("play/Jotaro.png", x, y, 20, 0.3, 500, 2, 1000) {
    // Move center downward, since we the turret head is slightly biased upward.
    Anchor.y += 8.0f / GetBitmapHeight();
    speed = 60;
}

void Jotaro::Hit(float damage) {
    HP -= damage;
    if (HP <= 0) {
        OnExplode();
        // Remove all armies' reference to target
        for (auto& it : lockedArmies)
            it->Target = nullptr;
        int x = static_cast<int>(floor(Position.x / PlayScene::BlockSize));
        int y = static_cast<int>(floor(Position.y / PlayScene::BlockSize));
        getPlayScene()->ClearMapState(x, y);
        getPlayScene()->DefenseGroup->RemoveObject(objectIterator);
        AudioHelper::PlayAudio("explosion.wav");
    }
}
void Jotaro::Update(float deltaTime) {
    Sprite::Update(deltaTime);
    PlayScene* scene = getPlayScene();
    if (!Enabled)
        return;
    if (scene->coco) {
        Rotation += 0.1;
        if (Rotation > 4.8) Rotation = -2;
        return;
    }
    if (!Target) {
        speed = 0;
        // Lock first seen target.
        // Can be improved by Spatial Hash, Quad Tree, ...
        // However simply loop through all enemies is enough for this program.
        int ey;
        for (auto& it : scene->ArmyGroup->GetObjects()) {
            Army* Tt = dynamic_cast<Army*>(it);
            if (Tt->dio) continue;
            ey = static_cast<int>(floor(it->Position.y / PlayScene::BlockSize));
            if (InShootingRange(it->Position)) {
                
                Target = dynamic_cast<Army*>(it);
                Target->lockedDefenses.push_back(this);
                lockedDefenseIterator = std::prev(Target->lockedDefenses.end());
                break;
            }
        }
        if (scene->DIOSAMA) Target = scene->DIOSAMA;
    }
    if (Target) {
        if (scene->DIOSAMA && scene->world) Target = scene->DIOSAMA;
        speed = 50;
        Rotation = UpdateRotation(deltaTime, Target->Position);
       // Rotation += 0.1;
        //if (Rotation > 4.8) Rotation = -2;
        std::cout << Rotation << "\n";
        // Bomb cannot shoot, so the reload time just set as coolDown.
        reload -= deltaTime;

        // TODO 2 (8/8): If bomb army is on the same block with target. Explode itself to deal damage to the target. Otherwise, move toward the target.
        if (Engine::Collider::IsCircleOverlap(Position, CollisionRadius+50, Target->Position, Target->CollisionRadius)) {
            if (reload <= 0) {
                reload = coolDown;
                CreateBullet(Target->Position);
            }
        }
        else {
            int dx = Target->Position.x - Position.x;
            int  dy = Target->Position.y - Position.y;
            double len = sqrt(pow(dx, 2) + pow(dy, 2));
            Velocity = speed * Engine::Point(dx / len, dy / len);
        }

    }

    Position.x += Velocity.x * deltaTime;
    Position.y += Velocity.y * deltaTime;
    }

void Jotaro::CreateBullet(Engine::Point pt) {
    int dx = pt.x - Position.x;
    int dy = pt.y - Position.y;
    double len = sqrt(pow(dx, 2) + pow(dy, 2));
    Engine::Point diff = Engine::Point(dx / len, dy / len);
    Engine::Point rotateDiff = Engine::Point(cos(Rotation - ALLEGRO_PI / 2), sin(Rotation - ALLEGRO_PI / 2));
    float rotation = atan2(rotateDiff.y, rotateDiff.x) + ALLEGRO_PI / 2;
    getPlayScene()->BulletGroup->AddNewObject(new Star_Platinum(Position, diff, rotation, this));
    AudioHelper::PlayAudio("ora.mp3");
}
float Jotaro::UpdateRotation(float deltaTime, Engine::Point target) {
    float ret;
    Engine::Point originRotation = Engine::Point(cos(Rotation - ALLEGRO_PI / 2), sin(Rotation - ALLEGRO_PI / 2));
    Engine::Point targetRotation = (target - Position).Normalize();
    float maxRotateRadian = rotateRadian * deltaTime;
    float cosTheta = originRotation.Dot(targetRotation);
    // Might have floating-point precision error.
    if (cosTheta > 1) cosTheta = 1;
    else if (cosTheta < -1) cosTheta = -1;

    if (cosTheta == -1) ret = ALLEGRO_PI;
    else {
        float radian = acos(cosTheta);
        Engine::Point rotation;
        if (abs(radian) <= maxRotateRadian)
            rotation = targetRotation;
        else
            rotation = ((abs(radian) - maxRotateRadian) * originRotation + maxRotateRadian * targetRotation) / radian;
        // Add 90 degrees (PI/2 radian), since we assume the image is oriented upward.
        ret = atan2(rotation.y, rotation.x) + ALLEGRO_PI / 2;
    }
    return ret;
}
