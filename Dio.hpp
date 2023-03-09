#ifndef Dio_hpp
#define Dio_hpp

#include <list>
#include <vector>
#include <string>

#include "Point.hpp"
#include "Role.hpp"
#include "Army.hpp";
class Bullet;
class PlayScene;
class Defense;

class Dio : public Army {
protected:
    Engine::Point target_tmp;
    
    float coolDown = 1;
    float reload = 0;
    float rotateRadian = 2 * ALLEGRO_PI;
    float shootRadius;
    std::list<Army*>::iterator lockedArmyIterator;
    //std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE> bgmInstance;
    //std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE> bgmDio;
public:
    ALLEGRO_TIMER* world_timer{};
    bool start;
    int data[9][2] = { {-60,-60},{0,-60},{60,-60},{-60,0},{0,0},{60,0},{-60,60},{0,60},{60,60} };
    Dio(float x, float y);
    void Update(float deltaTime) override;
    void CreateBullet(Engine::Point pt) override;
    
};

#endif /* Army_hpp */
