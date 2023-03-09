#ifndef ZAWARUDO_hpp
#define ZAWARUDO_hpp

#include "ArmyBullet.hpp"

class Army;
class Defense;
namespace Engine {
struct Point;
}  // namespace Engine

class ZAWARUDO : public ArmyBullet{
public:
    explicit ZAWARUDO(Engine::Point position, Engine::Point forwardDirection, float rotation, Army* parent);
    void OnExplode(Defense* defense) override;
};


#endif /* ArcherBullet_hpp */
