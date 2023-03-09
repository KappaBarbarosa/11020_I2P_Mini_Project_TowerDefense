#ifndef Vidio_HPP
#define Vidio_HPP
#include <allegro5/bitmap.h>
#include <memory>
#include <vector>

#include "Sprite.hpp"

class PlayScene;

class Vidio : public Engine::Sprite {
protected:
	PlayScene* getPlayScene();
	float timeTicks;
	std::vector<std::shared_ptr<ALLEGRO_BITMAP>> bmps;
	float timeSpan = 3;
	int count;
public:
	Vidio(float x, float y);
	void Update(float deltaTime) override;
	void Draw() const override;
};
#endif // EXPLOSIONEFFECT_HPP
