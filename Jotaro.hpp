#ifndef Jotaro_hpp
#define Jotaro_hpp

#include <allegro5/base.h>
#include <list>
#include <string>

#include "Defense.hpp"

class Army;
class PlayScene;

class Jotaro: public Defense {
private:
    float speed;
public:
    void CreateBullet(Engine::Point pt);   
    Jotaro(float x, float y);
    void Hit(float damage) override;
    void Update(float deltaTime) override;
    float UpdateRotation(float deltaTime, Engine::Point target);
};

#endif /* Defense_hpp */
