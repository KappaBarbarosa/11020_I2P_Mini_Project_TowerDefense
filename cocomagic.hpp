#ifndef Theworld_hpp
#define Theworld_hpp
#include <allegro5/allegro_primitives.h>
#include <allegro5/color.h>
#include "Role.hpp"
#include "Defense.hpp"
#include "Army.hpp"
#include "Cococute.hpp"
class Defense;
class Cococute;
class cocomagic : public Role {
public:
    float timeTicks;
    std::vector<std::shared_ptr<ALLEGRO_BITMAP>> bmps;
    float timeSpan = 3;
    int count;
    bool over;
    Cococute* Coco;
    cocomagic(Cococute* coco);
    void Update(float deltaTime) override;
    void health(Army* army);
    void Draw() const override;
    void gone() {
        if(over) getPlayScene()->magicGroup->RemoveObject(objectIterator);
    }
};

#endif /* WallDefense_hpp */
