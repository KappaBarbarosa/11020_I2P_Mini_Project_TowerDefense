#ifndef AraaraDefense_hpp
#define AraaraDefense_hpp

#include "Defense.hpp"

class AraaraDefense: public Defense {
public:
    AraaraDefense(float x, float y);
    void CreateBullet(Engine::Point pt) override;
    
};

#endif /* AraaraDefense_hpp */
