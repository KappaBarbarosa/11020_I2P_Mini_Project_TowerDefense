#ifndef Cocobullet_hpp
#define Cocobullet_hpp

#include "ArmyBullet.hpp"

class Army;
class Defense;
namespace Engine {
struct Point;
}  // namespace Engine

class Cocobullet : public ArmyBullet{
public:
    explicit Cocobullet(Engine::Point position, Engine::Point forwardDirection, float rotation, Army* parent);
    void OnExplode(Defense* defense) override;
    void Update(float deltatime) override;
};


#endif /* ArcherBullet_hpp */
