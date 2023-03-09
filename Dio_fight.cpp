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
#include "Dio_fight.hpp"
#include "ZAWARUDO.hpp"
#include "Resources.hpp"
#include "vidio.hpp"
#define TOP_LEFT 0
#define TOP_RIGHT 1
#define BOTTOM_LEFT 2
#define BOTTOM_RIGHT 3

#define LEFT 0
#define UP 1
#define RIGHT 2
#define DOWN 3
Dio_fight::Dio_fight(float x, float y) :Army("play/DIOSAMA.png", x, y, 20, 0, 100, 200, 4, 2000, false) {
    reachEndTime = 0;
    wallPos = Engine::Point(-1, -1);
    Velocity = Engine::Point(0, 0);
    start = true;
    time = false;
    tick = 0.0;
    for (int i = 1; i <= 45; i++) {
        bmps.push_back(Engine::Resources::GetInstance().GetBitmap("play/ZAWARUDO-" + std::to_string(i) + ".jpg"));
    }
}
void Dio_fight::Update(float deltaTime) {
    PlayScene* scene = getPlayScene(); 
    const int distThreshold = PlayScene::BlockSize / 32;

    if (isPreview) return;
    if (speed <= 100) speed = 100;
    int maxDis = INT_MAX;
    Defense* tgt = nullptr;
    for (auto& it : scene->DefenseGroup->GetObjects()) {
        Defense* jojo = dynamic_cast<Defense*>(it);
        int dis = ManHattanDistance(it->Position);
        if (dis < maxDis) {
            maxDis = dis;
            tgt = jojo;
        }
    }
    if (tgt) {
        Target = tgt;
        Target->lockedArmies.push_back(this);
        lockedArmyIterator = std::prev(Target->lockedArmies.end());
    }
    if (Target) {
        Rotation = UpdateRotation(deltaTime, Target->Position);
        CreateBullet(Target->Position);
    }
    if (time) start = false;
    if (start == false) tick += deltaTime;
    if (tick >= 8) {
        tick = 0;
        start = true;
        scene->world = false;
        AudioHelper::StopSample(getPlayScene()->bgmInstance);
        getPlayScene()->bgmInstance = AudioHelper::PlaySample("jojo.mp3");
    }
    theworld(deltaTime);
    Position.x += Velocity.x * deltaTime;
    Position.y += Velocity.y * deltaTime;
}


void Dio_fight::CreateBullet(Engine::Point pt) {
    if (fight == false) return;
    fight = false;
    int dx = pt.x - Position.x;
    int dy = pt.y - Position.y;
    double len = sqrt(pow(dx, 2) + pow(dy, 2));
    Engine::Point diff = Engine::Point(dx / len, dy / len);
    Engine::Point rotateDiff = Engine::Point(cos(Rotation - ALLEGRO_PI / 2), sin(Rotation - ALLEGRO_PI / 2));
    float rotation = atan2(rotateDiff.y, rotateDiff.x) + ALLEGRO_PI / 2;
    getPlayScene()->BulletGroup->AddNewObject(new ZAWARUDO(Position, diff, rotation, this));
    AudioHelper::PlayAudio("wry.mp3");
}
void Dio_fight::theworld(float deltaTime) {
    if (time == false) return;
    tick += deltaTime;
    time = false;
    getPlayScene()->world = true;
    AudioHelper::StopSample(getPlayScene()->bgmInstance);
    getPlayScene()->bgmInstance = AudioHelper::PlaySample("world.mp3", false, 5.0);
    getPlayScene()->EffectGroup->AddNewObject(new Vidio(768, 448));
    

     
}