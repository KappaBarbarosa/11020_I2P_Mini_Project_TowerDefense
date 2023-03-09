#include <cmath>
#include <string>

#include "Vidio.hpp"
#include "GameEngine.hpp"
#include "Group.hpp"
#include "IScene.hpp"
#include "PlayScene.hpp"
#include "Resources.hpp"
// TODO 3 (2/2): You can imitate the 2 files: '"ExplosionEffect.hpp', '"ExplosionEffect.cpp' to create a Shoot Effect.
PlayScene* Vidio::getPlayScene() {
	return dynamic_cast<PlayScene*>(Engine::GameEngine::GetInstance().GetActiveScene());
}
Vidio::Vidio(float x, float y) : Sprite("play/ZAWARUDO-1.jpg", x, y), timeTicks(0) {
	for (int i = 1; i <= 45; i++) {
		bmps.push_back(Engine::Resources::GetInstance().GetBitmap("play/ZAWARUDO-" + std::to_string(i) + ".jpg"));
	}
}
void Vidio::Update(float deltaTime) {

	timeTicks += deltaTime;
	if (timeTicks >= 4) {
		getPlayScene()->EffectGroup->RemoveObject(objectIterator);
		return;
	}
	int phase = floor(timeTicks / 4 * bmps.size());
	bmp = bmps[phase];
	Sprite::Update(deltaTime);
}
void Vidio::Draw() const {
	al_draw_tinted_scaled_rotated_bitmap(bmp.get(), Tint, Anchor.x * GetBitmapWidth(), Anchor.y * GetBitmapHeight(),
		Position.x, Position.y, 20*Size.x/ GetBitmapWidth(), 20*Size.y/ GetBitmapHeight(), Rotation, 0);
}