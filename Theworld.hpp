#ifndef Theworld_hpp
#define Theworld_hpp

#include "Role.hpp"
#include "Defense.hpp"
class Defense;
class theworld : public Role {
public:
    bool over;
    theworld(float x, float y);
    void Update(float deltaTime) override;
    void THEWORLD(Defense* defencse);
    void timeup();
    std::list<Defense*> lockedDefenses;
};

#endif /* WallDefense_hpp */
