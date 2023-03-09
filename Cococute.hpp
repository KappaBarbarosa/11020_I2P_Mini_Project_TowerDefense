#ifndef Cococute_hpp
#define Cococute_hpp

#include <list>
#include <vector>
#include <string>
#include "cocomagic.hpp"
#include "Point.hpp"
#include "Role.hpp"
#include "Army.hpp"
class Bullet;
class PlayScene;
class Defense;
class cocomagic;
class Cococute : public Army {
protected:
    Engine::Point target_tmp;
    bool start;
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
    // up down left  right
    Cococute(float x, float y);
    cocomagic* magic;
    void Hit(float damage) override;
    void Update(float deltaTime) override;
    void CreateBullet(Engine::Point pt) override;
    bool InCocoRange(Engine::Point obj) {
        return (pow(abs(Position.x - obj.x), 2) + pow(abs(Position.y - obj.y), 2) <= pow(300, 2));
    }

};
#endif 