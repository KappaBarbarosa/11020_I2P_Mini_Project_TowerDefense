#ifndef AraaraBullet_hpp
#define AraaraBullet_hpp

#include "Bullet.hpp"

class Army;
class Defense;
namespace Engine {
struct Point;
}  // namespace Engine

class AraaraBullet : public Bullet {
public:
    explicit AraaraBullet(Engine::Point position, Engine::Point forwardDirection, float rotation, Defense* parent);
    void Update(float deltaTime) override;
};

#endif /* AraaraBullet_hpp */
