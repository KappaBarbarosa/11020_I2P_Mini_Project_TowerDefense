#include <allegro5/allegro_primitives.h>
#include <allegro5/color.h>
#include <cmath>
#include <random>
#include <string>
#include <vector>
#include <algorithm>
#include <iostream>
#include "Collider.hpp"
#include "AudioHelper.hpp"
#include "Bullet.hpp"
#include "DirtyEffect.hpp"
#include "Army.hpp"
#include "ArmyBullet.hpp"
#include "ExplosionEffect.hpp"
#include "GameEngine.hpp"
#include "Group.hpp"
#include "IScene.hpp"
#include "LOG.hpp"
#include "PlayScene.hpp"
#include "Defense.hpp"
#include "Cococute.hpp"
#include "ZAWARUDO.hpp"
#include "Resources.hpp"
#include "vidio.hpp"
#include "Cocobullet.hpp"
#include "cocomagic.hpp";
#define TOP_LEFT 0
#define TOP_RIGHT 1
#define BOTTOM_LEFT 2
#define BOTTOM_RIGHT 3

#define LEFT 0
#define UP 1
#define RIGHT 2
#define DOWN 3
Cococute::Cococute(float x, float y) :Army("play/cococute.png", x, y, 20, 1, 50, 100, 3, 3 * PlayScene::BlockSize, false) {
    reachEndTime = 0;
    wallPos = Engine::Point(-1, -1);
    Velocity = Engine::Point(0, 0);
    start = true;
}
void Cococute::Update(float deltaTime) {
    // PlayScene
    
    PlayScene* scene = getPlayScene();
    if (scene->world)return;
    const int distThreshold = PlayScene::BlockSize / 32;

    if (isPreview) return;
    if (start) {
        magic = new cocomagic(this);
        scene->magicGroup->AddNewObject(magic);
        start = false;
    }
    // position
    int x = static_cast<int>(floor(Position.x / PlayScene::BlockSize));
    int y = static_cast<int>(floor(Position.y / PlayScene::BlockSize));
    if (scene->coco) {
        Rotation += 0.1;
        if (Rotation > 4.8) Rotation = -2;
        return;
    }
    if (region == 5) {
        if (!Target) {
            // Lock closet target
            // Can be improved by Spatial Hash, Quad Tree, ...
            // However simply loop through all enemies is enough for this program.
            int maxDis = INT_MAX;
            Defense* tgt = nullptr;
            for (auto& it : scene->DefenseGroup->GetObjects()) {
                int dis = ManHattanDistance(it->Position);
                if (dis < maxDis) {
                    maxDis = dis;
                    tgt = dynamic_cast<Defense*>(it);
                }
            }
            if (tgt) {
                Target = tgt;
                Target->lockedArmies.push_back(this);
                lockedArmyIterator = std::prev(Target->lockedArmies.end());
            }
        }
        if (Target) {
            Rotation = UpdateRotation(deltaTime, Target->Position);
            // Shoot reload.
            reload -= deltaTime;
            if (reload <= 0) {
                // shoot.
                reload = coolDown;
                if (InShootingRange(Target->Position))
                    CreateBullet(Target->Position);
            }
            if (InShootingRange(Target->Position)) {
                Velocity = Engine::Point(0, 0);
            }
            else if (InCocoRange(Target->Position)) {
                int dx = Target->Position.x - Position.x;
                int dy = Target->Position.y - Position.y;
                double len = sqrt(pow(dx, 2) + pow(dy, 2));
                Velocity = 1.2 * speed * Engine::Point(dx / len, dy / len);
            }
            else {
                float tx = Target->Position.x;
                float ty = Target->Position.y;

                if (dir == 1) {
                    if (abs(tx - Position.x) > distThreshold) {
                        Velocity = (Position.x < tx) ? Engine::Point(1, 0) : Engine::Point(-1, 0);
                    }
                    else {
                        Position.x = tx;
                        Velocity = (Position.y < ty) ? Engine::Point(0, 1) : Engine::Point(0, -1);
                    }
                }
                else {
                    if (abs(ty - Position.y) > distThreshold) {
                        Velocity = (Position.y < ty) ? Engine::Point(0, 1) : Engine::Point(0, -1);
                    }
                    else {
                        Position.y = ty;
                        Velocity = (Position.x < tx) ? Engine::Point(1, 0) : Engine::Point(-1, 0);
                    }
                }
            }
        }
    }
    else { // region != 5
        //for (int i = 0;i < 4;i++) std::cout << scene->brokenWall[i].empty();
        //std::cout << "\n";
        CalcRegion(x, y);
        if (!movingToWall) {
            // top
            if (region == 1 || region == 2 || region == 3) {
                if (!scene->brokenWall[UP].empty()) {
                    movingToWall = true;
                    int minDis = INT_MAX;
                    for (auto wall : scene->brokenWall[UP]) {
                        int dis = ManHattanDistance(wall * PlayScene::BlockSize);
                        if (minDis > dis) {
                            minDis = dis;
                            wallPos = wall * PlayScene::BlockSize + Engine::Point(PlayScene::BlockSize / 2, PlayScene::BlockSize / 2);
                        }
                    }
                    dir = 0;
                }
            }
            // down
            if (region == 7 || region == 8 || region == 9) {
                if (!scene->brokenWall[DOWN].empty()) {
                    movingToWall = true;
                    int minDis = INT_MAX;
                    for (auto wall : scene->brokenWall[DOWN]) {
                        int dis = ManHattanDistance(wall * PlayScene::BlockSize);
                        if (minDis > dis) {
                            minDis = dis;
                            wallPos = wall * PlayScene::BlockSize + Engine::Point(PlayScene::BlockSize / 2, PlayScene::BlockSize / 2);
                        }
                    }
                    dir = 0;
                }
            }
            // left
            if (region == 1 || region == 4 || region == 7) {
                if (!scene->brokenWall[LEFT].empty()) {
                    movingToWall = true;
                    int minDis = INT_MAX;
                    for (auto wall : scene->brokenWall[LEFT]) {
                        int dis = ManHattanDistance(wall * PlayScene::BlockSize);
                        if (minDis > dis) {
                            minDis = dis;
                            wallPos = wall * PlayScene::BlockSize + Engine::Point(PlayScene::BlockSize / 2, PlayScene::BlockSize / 2);
                        }
                    }
                    dir = 1;
                }
            }
            // right
            if (region == 3 || region == 6 || region == 9) {
                if (!scene->brokenWall[RIGHT].empty()) {
                    movingToWall = true;
                    int minDis = INT_MAX;
                    for (auto wall : scene->brokenWall[RIGHT]) {
                        int dis = ManHattanDistance(wall * PlayScene::BlockSize);
                        if (minDis > dis) {
                            minDis = dis;
                            wallPos = wall * PlayScene::BlockSize + Engine::Point(PlayScene::BlockSize / 2, PlayScene::BlockSize / 2);
                        }
                    }
                    dir = 1;
                }
            }
        }

        // there's no broken wall
        if (!movingToWall) {
            if (!Target) {
                // Lock closet wall
                // Can be improved by Spatial Hash, Quad Tree, ...
                // However simply loop is enough for this program.
                int maxDis = INT_MAX;
                Defense* tgt = nullptr;
                for (auto& it : scene->WallGroup->GetObjects()) {
                    int dis = ManHattanDistance(it->Position);
                    if (dis < maxDis) {
                        maxDis = dis;
                        tgt = dynamic_cast<Defense*>(it);
                        //s::cout << "wall!!\n";
                    }
                }
                if (tgt) {
                    Target = tgt;
                    Target->lockedArmies.push_back(this);
                    lockedArmyIterator = std::prev(Target->lockedArmies.end());
                }
            }
            else {
                Rotation = UpdateRotation(deltaTime, Target->Position);

                // Shoot reload.
                reload -= deltaTime;
                if (reload <= 0) {
                    // shoot.
                    reload = coolDown;
                    if (InShootingRange(Target->Position))
                        CreateBullet(Target->Position);
                }
                if (InShootingRange(Target->Position)) {
                    Velocity = Engine::Point(0, 0);
                }
                else if (InCocoRange(Target->Position)){
                    int dx = Target->Position.x - Position.x;
                    int dy = Target->Position.y - Position.y;
                    double len = sqrt(pow(dx, 2) + pow(dy, 2));
                    Velocity = 1.2*speed * Engine::Point(dx / len, dy / len);
                }
                else {
                    float tx = Target->Position.x;
                    float ty = Target->Position.y;

                    if (dir == 0) {
                        if (abs(tx - Position.x) > distThreshold) {
                            Velocity = (Position.x < tx) ? Engine::Point(1, 0) : Engine::Point(-1, 0);
                        }
                        else {
                            Position.x = tx;
                            Velocity = (Position.y < ty) ? Engine::Point(0, 1) : Engine::Point(0, -1);
                        }
                    }
                    else {
                        if (abs(ty - Position.y) > distThreshold) {
                            Velocity = (Position.y < ty) ? Engine::Point(0, 1) : Engine::Point(0, -1);
                        }
                        else {
                            Position.y = ty;
                            Velocity = (Position.x < tx) ? Engine::Point(1, 0) : Engine::Point(-1, 0);
                        }
                    }
                }
            }
        }
        else {
            if (Target) {
                Target = nullptr;
            }
            Rotation = UpdateRotation(deltaTime, wallPos);
            // Shoot reload.
            reload = coolDown;
            // position
            float wx = wallPos.x;
            float wy = wallPos.y;

            if (abs(wx - Position.x) < distThreshold && abs(wy - Position.y) < distThreshold) {
                Position = wallPos;
                movingToWall = false;
                region = 5;
                Velocity = Engine::Point(0, 0);
            }
            
            else {
                if (dir == 0) {
                    if (abs(wx - Position.x) > distThreshold) {
                        Velocity = (Position.x < wx) ? Engine::Point(1, 0) : Engine::Point(-1, 0);
                    }
                    else {
                        Position.x = wx;
                        Velocity = (Position.y < wy) ? Engine::Point(0, 1) : Engine::Point(0, -1);
                    }
                }
                else {
                    if (abs(wy - Position.y) > distThreshold) {
                        Velocity = (Position.y < wy) ? Engine::Point(0, 1) : Engine::Point(0, -1);
                    }
                    else {
                        Position.y = wy;
                        Velocity = (Position.x < wx) ? Engine::Point(1, 0) : Engine::Point(-1, 0);
                    }
                }
            }
        }
    }
    Velocity = Velocity * speed;

    Position.x += Velocity.x * deltaTime;
    Position.y += Velocity.y * deltaTime;
}

void Cococute::CreateBullet(Engine::Point pt) {

    int dx = pt.x - Position.x;
    int dy = pt.y - Position.y;
    double len = sqrt(pow(dx, 2) + pow(dy, 2));
    Engine::Point diff = Engine::Point(dx / len, dy / len);
    Engine::Point rotateDiff = Engine::Point(cos(Rotation - ALLEGRO_PI / 2), sin(Rotation - ALLEGRO_PI / 2));
    float rotation = atan2(rotateDiff.y, rotateDiff.x) + ALLEGRO_PI / 2;
    getPlayScene()->BulletGroup->AddNewObject(new Cocobullet(Position, diff, rotation, this));
    AudioHelper::PlayAudio("laser.wav");
}
void Cococute::Hit(float damage) {
    HP -= damage;
    if (HP <= 0) {
        OnExplode();
        // Remove all Defense's reference to target.
        for (auto& it : lockedDefenses)
            it->Target = nullptr;
        magic->over = true;
        getPlayScene()->ArmyGroup->RemoveObject(objectIterator);
        
        AudioHelper::PlayAudio("explosion.wav");
    }
}