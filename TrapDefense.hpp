#ifndef TrapDefense_hpp
#define TrapDefense_hpp

#include "Defense.hpp"
#include "Army.hpp"

class TrapDefense: public Defense {

public:
    bool show;
    TrapDefense(float x, float y);
    void CreateBullet(Engine::Point pt) override;
    void Draw() const override;
    void Update(float deltatime) override;
    void Hit(float damage) override;
    void trapon(Army* army);
    bool start;
    float span;
};

#endif /* CannonDefense_hpp */
