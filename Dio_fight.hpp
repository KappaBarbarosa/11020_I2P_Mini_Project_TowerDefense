#ifndef spell_hpp
#define spell_hpp

#include <list>
#include <vector>
#include <string>

#include "Point.hpp"
#include "Role.hpp"
#include "Army.hpp"
class Bullet;
class PlayScene;
class Defense;

class Dio_fight : public Army {
protected:
    Engine::Point target_tmp;
    
    float coolDown = 1;
    float reload = 0;
    float rotateRadian = 2 * ALLEGRO_PI;
    float shootRadius;
    std::vector<std::shared_ptr<ALLEGRO_BITMAP>> bmps;
    std::shared_ptr<ALLEGRO_BITMAP> dmp;
    std::list<Army*>::iterator lockedArmyIterator;
    //std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE> bgmInstance;
    //std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE> bgmDio;
public:
    ALLEGRO_TIMER* world_timer{};
    bool start;
    int data[9][2] = { {-60,-60},{0,-60},{60,-60},{-60,0},{0,0},{60,0},{-60,60},{0,60},{60,60} };
    // up down left  right
    Dio_fight(float x, float y);
    void Update(float deltaTime) override;
    void CreateBullet(Engine::Point pt) override;
    float tick;
    void theworld(float deltaTime);
};
#endif 