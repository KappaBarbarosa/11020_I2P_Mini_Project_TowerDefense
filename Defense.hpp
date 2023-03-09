#ifndef Defense_hpp
#define Defense_hpp

#include <allegro5/base.h>
#include <list>
#include <string>

#include "Role.hpp"

class Army;
class PlayScene;

class Defense: public Role {
protected:
    float shootRadius;
    float coolDown;
    float reload = 0;
    float rotateRadian = 2 * ALLEGRO_PI;    
    std::list<Defense*>::iterator lockedDefenseIterator;
    virtual void CreateBullet(Engine::Point pt) = 0;
    
public:
    int id;
    bool coconfuse;
    bool world = false;
    bool Enabled = true;
    bool Preview = false;
    Army* Target = nullptr;
    Defense* cocoTarget = nullptr;
    std::list<Army*> lockedArmies;
    std::list<Defense*> lockedDefense;
    Defense(std::string imgDefense, float x, float y, float radius, float coolDown, int hp, int id, float shootRadius);
    void Hit(float damage) override;
    void confuse();
    void Update(float deltaTime) override;
    void CocoUpdate(float deltaTime);
    void Draw() const override;
    bool InShootingRange(Engine::Point obj);
};

#endif /* Defense_hpp */
