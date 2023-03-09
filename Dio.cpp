#include <allegro5/allegro_primitives.h>
#include <allegro5/color.h>
#include <cmath>
#include <random>
#include <string>
#include <vector>
#include <algorithm>
#include <iostream>

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
#include "Dio.hpp"
#include "Theworld.hpp"
#include "vidio.hpp"
#define TOP_LEFT 0
#define TOP_RIGHT 1
#define BOTTOM_LEFT 2
#define BOTTOM_RIGHT 3

#define LEFT 0
#define UP 1
#define RIGHT 2
#define DOWN 3
Dio::Dio(float x, float y) :Army("play/iceevy.png", x, y,20,0,0,INFINITY,5,0,true) {
    reachEndTime = 0;
    wallPos = Engine::Point(-1, -1);
    Velocity = Engine::Point(0, 0);
    world_timer = al_create_timer(1.0f);
    start = true;
    //for(int i=0;i<44;i++)
}
void Dio::CreateBullet(Engine::Point pt) {}

void Dio::Update(float deltaTime) {
    PlayScene* scene = getPlayScene(); 
    if (isPreview) return ;
    if (start) {
        al_start_timer(world_timer);
        for (int i = 0;i < 9;i++) getPlayScene()->WorldGroup->AddNewObject(new theworld(Position.x + data[i][0], Position.y + data[i][1]));
        AudioHelper::PlaySample("ice.mp3",false,5.0);

        start = false;
    }
    // position
    int x = static_cast<int>(floor(Position.x / PlayScene::BlockSize));
    int y = static_cast<int>(floor(Position.y / PlayScene::BlockSize));
    if (al_get_timer_count(world_timer) == 2) {
        al_start_timer(world_timer);
        int cnt = 0;
        theworld* tgt;
        for (auto& it : scene->WorldGroup->GetObjects()) {
            if (cnt == 9) break;
            tgt = dynamic_cast<theworld*>(it);
            tgt->timeup();
            cnt++;
        }
        al_destroy_timer(world_timer);
        scene->ArmyGroup->RemoveObject(objectIterator);
    }
    const int distThreshold = PlayScene::BlockSize / 32;
    Velocity = Velocity * speed;

    Position.x += Velocity.x * deltaTime;
    Position.y += Velocity.y * deltaTime;
}


