#ifndef KabiArmy_hpp
#define KabiArmy_hpp

#include "Army.hpp"

class KabiArmy: public Army {
public:
    KabiArmy(float x, float y);
    void Update(float deltaTime) override;
    void CreateBullet(Engine::Point pt) override;
    void Hit(float damage) override;
    void OnExplode() override;
};

#endif /* KabiArmy_hpp */
