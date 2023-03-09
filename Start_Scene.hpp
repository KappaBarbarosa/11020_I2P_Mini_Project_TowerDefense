#ifndef STARTSCENE_HPP
#define STARTSCENE_HPP
#include <allegro5/allegro_audio.h>
#include <memory>
#include "IScene.hpp"

class StartScene final : public Engine::IScene {
private:
	std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE> bgmInstance;
public:
	explicit StartScene() = default;
	void Initialize() override;
	void Terminate() override;
	void PlayOnClick(int stage);
	ALLEGRO_BITMAP* bmp = NULL;
};

#endif // STARTSCENE_HPP
