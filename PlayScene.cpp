#include <allegro5/allegro.h>
#include <algorithm>
#include <random>
#include <cmath>
#include <fstream>
#include <functional>
#include <vector>
#include <queue>
#include <string>
#include <memory>
#include <numeric> 
#include <iostream>
// Fundamental
#include "AudioHelper.hpp"
#include "DirtyEffect.hpp"
#include "GameEngine.hpp"
#include "Group.hpp"
#include "IObject.hpp"
#include "Image.hpp"
#include "Label.hpp"
#include "Plane.hpp"
#include "PlayScene.hpp"
#include "Resources.hpp"
#include "Sprite.hpp"
#include "LOG.hpp"

// Button
#include "ArmyButton.hpp"

// Army
#include "BombArmy.hpp"
#include "ArcherArmy.hpp"
#include "KabiArmy.hpp"
#include "Dio.hpp"
#include "Coco.hpp"
#include "Dio_fight.hpp"
#include "Cococute.hpp"
// Defense
#include "CannonDefense.hpp"
#include "AraaraDefense.hpp"
#include "WallDefense.hpp"
#include "TrapDefense.hpp"
#include "Jotaro.hpp"

#define LEFT 0
#define RIGHT 1
#define UP 2
#define DOWN 3
#define WALL_SIZE 4
#define MAX_ARMY_AMOUNT 6

// TODO 2 (4/8) : Add the BombArmy when click the imageButton and place it. You can search for the ArcherArmy to know where to add.

bool PlayScene::DebugMode = false;
const std::vector<Engine::Point> PlayScene::directions = { Engine::Point(-1, 0), Engine::Point(0, -1), Engine::Point(1, 0), Engine::Point(0, 1) };
const int PlayScene::MapWidth = 24, PlayScene::MapHeight = 12;//50;//13;
const int PlayScene::BlockSize = 64;
const float PlayScene::DangerTime = 7.61;
// TODO 4 (2/3): Set the code sequence correctly.
const std::vector<int> PlayScene::code = { ALLEGRO_KEY_UP,ALLEGRO_KEY_UP,ALLEGRO_KEY_DOWN,ALLEGRO_KEY_DOWN,ALLEGRO_KEY_LEFT,ALLEGRO_KEY_RIGHT,ALLEGRO_KEY_ENTER};
const std::vector<int> PlayScene::dio_code = { ALLEGRO_KEY_K,ALLEGRO_KEY_O,ALLEGRO_KEY_N,ALLEGRO_KEY_O,ALLEGRO_KEY_D,ALLEGRO_KEY_I,ALLEGRO_KEY_O,ALLEGRO_KEY_D,ALLEGRO_KEY_A };
Engine::Point PlayScene::GetClientSize() {
	return Engine::Point(MapWidth * BlockSize, MapHeight * BlockSize);
}
void PlayScene::Initialize() {
    coco = false;
	mapState.clear();
	keyStrokes.clear();
	ticks = 0;
	deathCountDown = -1;
	SpeedMult = 1;
    MapId = 1;
    for (int i = 0;i < 4;i++) brokenWall[i].clear();
	// Add groups from bottom to top.
	AddNewObject(TileMapGroup = new Group());
    AddNewObject(magicGroup = new Group());
    AddNewObject(WallGroup = new Group());
    AddNewObject(TrapGroup = new Group());
    
    AddNewObject(DefenseGroup = new Group());
    AddNewObject(WorldGroup = new Group());
    AddNewObject(ArmyGroup = new Group());
	AddNewObject(DebugIndicatorGroup = new Group());
	AddNewObject(BulletGroup = new Group());

    AddNewObject(GroundEffectGroup = new Group());
	AddNewObject(EffectGroup = new Group());
    gametimer = al_create_timer(1.0f / 60);
    start = true;
	// Should support buttons.
	AddNewControlObject(UIGroup = new Group());
	ReadMap();
	ConstructUI();
	imgTarget = new Engine::Image("play/target.png", 0, 0, BlockSize, BlockSize);
	imgTarget->Visible = false;
	preview = nullptr;
	UIGroup->AddNewObject(imgTarget);
	// Preload Lose Scene

	deathBGMInstance = Engine::Resources::GetInstance().GetSampleInstance("astronomia.ogg");
	Engine::Resources::GetInstance().GetBitmap("lose/benjamin-happy.png");
	// Start BGM.
    if (!mute)
        bgmInstance = AudioHelper::PlaySample("play.ogg", true, AudioHelper::BGMVolume);
    else
        bgmInstance = AudioHelper::PlaySample("play.ogg", true, 0.0);
}
void PlayScene::Terminate() {
    AudioHelper::StopSample(bgmInstance);
    bgmInstance = std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE>();
	AudioHelper::StopSample(deathBGMInstance);
	deathBGMInstance = std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE>();
    al_destroy_timer(gametimer);
	IScene::Terminate();
}
void PlayScene::Update(float deltaTime) {
	if (SpeedMult == 0)
		deathCountDown = -1;
	else if (deathCountDown != -1)
		SpeedMult = 1;

	for (int i = 0; i < SpeedMult; i++) {
	    IScene::Update(deltaTime);
		ticks += deltaTime;
	}

    // Win
    if (DefenseGroup->GetObjects().empty()) {
        Engine::GameEngine::GetInstance().ChangeScene("win");
    }
    
    // Lose
    bool armyEmpty = true;
    for (int i=0; i<totalArmy+1; i++) {
        if (armyAmount[i] > 0) {
            armyEmpty = false;
            break;
        }
    }
    if (!ArmyGroup->GetObjects().empty()) armyEmpty = false;
    if (armyEmpty) {
        Engine::GameEngine::GetInstance().ChangeScene("lose");
    }
    
	if (preview) {
		preview->Position = Engine::GameEngine::GetInstance().GetMousePosition();
		// To keep responding when paused.
		preview->Update(deltaTime);
	}
}
void PlayScene::Draw() const {
	IScene::Draw();
}
void PlayScene::OnMouseDown(int button, int mx, int my) {
	if ((button & 1) && !imgTarget->Visible && preview) {
		// Cancel army construct.
		UIGroup->RemoveObject(preview->GetObjectIterator());
		preview = nullptr;
	}
	IScene::OnMouseDown(button, mx, my);
}
void PlayScene::OnMouseMove(int mx, int my) {
	IScene::OnMouseMove(mx, my);
	const int x = mx / BlockSize;
	const int y = my / BlockSize;
    
	if (!preview || x < 0 || x >= MapWidth || y < 0 || y >= MapHeight) {
		imgTarget->Visible = false;
		return;
	}
    
    imgTarget->Visible = true;
    imgTarget->Position.x = x * BlockSize;
    imgTarget->Position.y = y * BlockSize;
}
void PlayScene::OnMouseUp(int button, int mx, int my) {
	IScene::OnMouseUp(button, mx, my);

    if (!imgTarget->Visible)
        return;
	const int x = mx / BlockSize;
	const int y = my / BlockSize;
	if(x < 0 || x >= MapWidth || y < 0 || y >= MapHeight)
	    return;
	if (button & 1) {
		if (!CheckOccupied(x, y) || (preview->id==5 &&Checkworld(x,y))) {
			if (!preview)
				return;
            ReduceAmount(preview->id);
            int remainId = armyAmount[preview->id]> 0 ? preview->id : -1;
			// Remove Preview.
			preview->GetObjectIterator()->first = false;
            UIGroup->RemoveObject(preview->GetObjectIterator());
            //if (preview->id == 4) coco = true;
			// Construct real turret.
			preview->Position.x = x * BlockSize + BlockSize / 2;
			preview->Position.y = y * BlockSize + BlockSize / 2;
			preview->Enabled = true;
            preview->isPreview = false;
			preview->Tint = al_map_rgba(255, 255, 255, 255);
            preview->CalcRegion(x, y);
            if (preview->id == 4) {
                DIOSAMA = preview;
                AudioHelper::StopSample(bgmInstance);
                bgmInstance = AudioHelper::PlaySample("jojo.mp3", true, AudioHelper::BGMVolume);
            }
			ArmyGroup->AddNewObject(preview);
			// To keep responding when paused.
			preview->Update(0);
			// Remove Preview.
			preview = nullptr;
            if (start)al_start_timer(gametimer);
            // if amount > 0, keep the preview exists
            if (remainId != -1) {
                if (remainId == 0)
                    preview = new ArcherArmy(0, 0);
                else if (remainId == 1)
                    preview = new BombArmy(0, 0);
                else if (remainId == 2)
                    preview = new KabiArmy(0, 0);
                else if (remainId == 5)
                    preview = new Dio(0, 0);
                else if (remainId == 4)
                    preview = new Dio_fight(0, 0);
                else if (remainId == 3)
                    preview = new Cococute(0, 0);

                preview->Position = Engine::GameEngine::GetInstance().GetMousePosition();
                preview->Tint = al_map_rgba(255, 255, 255, 200);
                preview->Enabled = false;
                preview->isPreview = true;
                UIGroup->AddNewObject(preview);
            }

			OnMouseMove(mx, my);
		}
		else{
            Engine::Sprite* sprite;
            GroundEffectGroup->AddNewObject(sprite = new DirtyEffect("play/target-invalid.png", 1, x * BlockSize + BlockSize / 2, y * BlockSize + BlockSize / 2));
            sprite->Rotation = 0;
			OnMouseMove(mx, my);
		}
	}
}
void PlayScene::OnKeyDown(int keyCode) {
	IScene::OnKeyDown(keyCode);
	if (keyCode == ALLEGRO_KEY_TAB) {
        if (DebugMode) DebugMode = false;
        else DebugMode = true;
	}
    else if(keyCode == ALLEGRO_KEY_T)
        Engine::GameEngine::GetInstance().ChangeScene("win");
	else {
        dio_keyStrokes.push_back(keyCode);
		keyStrokes.push_back(keyCode);
		if (keyStrokes.size() > code.size()) keyStrokes.pop_front();
        if (dio_keyStrokes.size() > dio_code.size()) dio_keyStrokes.pop_front();
        int cnt = 0,diocnt=0;
        for (auto n : keyStrokes) {
            if (n != code[cnt++]) break;
        } if(cnt==code.size()) EffectGroup->AddNewObject(new Plane());
        for (auto n : dio_keyStrokes) {
            if (n != dio_code[diocnt++]) break;
        } if (diocnt == dio_code.size())UIArmyAmount[4]->Text = "x" + std::to_string(++armyAmount[4]);
        // TODO 4 (3/3): Check whether the input sequence corresponds to the code.
        // Active a plane : EffectGroup->AddNewObject(new Plane());
	}
	if (keyCode == ALLEGRO_KEY_Q) {
		// Hotkey for ArcherArmy.
		UIBtnClicked(0);
	}
	else if (keyCode == ALLEGRO_KEY_A) {
		// Hotkey for BombArmy.
        if (DIOSAMA) {
            DIOSAMA->Dio_move(0);
        }
	}
	else if (keyCode == ALLEGRO_KEY_W) {
		// Hotkey for ...
        if (DIOSAMA) {
            DIOSAMA->Dio_move(2);
        }
	}
    else if (keyCode == ALLEGRO_KEY_N) {
        // Hotkey for ...
        if (DIOSAMA) {
            DIOSAMA->time = true;
        }
    }
	else if (keyCode == ALLEGRO_KEY_D) {
		// Hotkey for ...
        if (DIOSAMA) {
            DIOSAMA->Dio_move(1);
        }
	}
    else if (keyCode == ALLEGRO_KEY_S) {
        // Hotkey for ...
        if (DIOSAMA) {
            DIOSAMA->Dio_move(3);
        }
    }
    else if (keyCode == ALLEGRO_KEY_R) {
        // Hotkey for ...
        if (DIOSAMA) {
            DIOSAMA->fight = true;
        }
    }
    else if (keyCode == ALLEGRO_KEY_M) {
        // mute
        mute = !mute;
        if (!mute)
            AudioHelper::ChangeSampleVolume(bgmInstance, AudioHelper::BGMVolume);
        else
            AudioHelper::ChangeSampleVolume(bgmInstance, 0.0);
    }
	else if (keyCode >= ALLEGRO_KEY_0 && keyCode <= ALLEGRO_KEY_9) {
		// Hotkey for Speed up.
		SpeedMult = keyCode - ALLEGRO_KEY_0;
	}
}

void PlayScene::ReduceAmount(int id) {
    armyAmount[id]--;
    UIArmyAmount[id]->Text = "x" + std::to_string(armyAmount[id]);
}
void PlayScene::ReadMap() {
	std::string filename = std::string("resources/map") + std::to_string(MapId) + ".txt";
	// Read map file.
	char c;
	std::vector<TileType> mapData;
	std::ifstream fin(filename);
	while (fin >> c) {
        // 0: floor
        // 1: wall
        // 2: canon
        //std::cout << c;
		switch (c) {
		case '0': mapData.push_back(TILE_FLOOR); break;
		case '1': mapData.push_back(TILE_WALL); break;
        case '2': mapData.push_back(TILE_CANNON); break;
        case '3': mapData.push_back(TILE_ARAARA); break;
        case '4': mapData.push_back(TILE_TRAP); break;
        case '5': mapData.push_back(TILE_JOJO); break;
		case '\n':
		case '\r':
			if (static_cast<int>(mapData.size()) / MapWidth != 0)
				throw std::ios_base::failure("Map data is corrupted.");
			break;
		default: throw std::ios_base::failure("Map data is corrupted.");
		}
	}
	fin.close();
	// Validate map data.
	if (static_cast<int>(mapData.size()) != MapWidth * MapHeight)
		throw std::ios_base::failure("Map data is corrupted.");
	// Store map in 2d array.
	mapState = std::vector<std::vector<TileType>>(MapHeight, std::vector<TileType>(MapWidth));
    int cornerIdx = 0;
	for (int i = 0; i < MapHeight; i++) {
		for (int j = 0; j < MapWidth; j++) {
			const TileType md = mapData[i * MapWidth + j];
            
            TileMapGroup->AddNewObject(new Engine::Image("play/deep-pink.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
            switch (md) {
                case TILE_WALL:
                    WallGroup->AddNewObject(new WallDefense(j * BlockSize + BlockSize / 2, i * BlockSize + BlockSize / 2));
                    if (j >= 2) {
                        if (mapState[i][j-2] == TILE_FLOOR && mapState[i][j-1] == TILE_WALL) {
                            corners[cornerIdx++] = Engine::Point(j-1, i);
                        }
                    }
                    break;
                case TILE_CANNON:
                    DefenseGroup->AddNewObject(new CannonDefense(j * BlockSize + BlockSize / 2, i * BlockSize + BlockSize / 2));
                    break;
                case TILE_ARAARA:
                    DefenseGroup->AddNewObject(new AraaraDefense(j * BlockSize + BlockSize / 2, i * BlockSize + BlockSize / 2));
                    break;
                case TILE_JOJO:
                    DefenseGroup->AddNewObject(new Jotaro(j * BlockSize + BlockSize / 2, i * BlockSize + BlockSize / 2));
                    break;
                case TILE_TRAP:
                    TrapGroup->AddNewObject(new TrapDefense(j * BlockSize + BlockSize / 2, i * BlockSize + BlockSize / 2));
                    break;
                case TILE_FLOOR:
                    if (j <= MapWidth-2 && j >= 2) {
                        if (mapState[i][j-2] == TILE_WALL && mapState[i][j-1] == TILE_WALL) {
                            corners[cornerIdx++] = Engine::Point(j-1, i);
                        }
                    }
                    break;
                default:
                    throw std::ios_base::failure("Map data is corrupted.");
            }
            mapState[i][j] = md;
		}
	}
    
    // There must be four corners in the map
    if (cornerIdx != 4)
        throw std::ios_base::failure("Corner size is wrong.");
}

void PlayScene::ConstructUI() {
	// Background
	UIGroup->AddNewObject(new Engine::Image("play/sand.png", 0, 64*MapHeight, 1536, 128));
    
    // TODO 2 (3/8) : Construct the select button for bomb army.
    ConstructButton(0, ArmyImage[0]);
    ConstructButton(1, ArmyImage[1]);
    ConstructButton(2, ArmyImage[2]);
    ConstructButton(3, ArmyImage[3]);
    ConstructButton(4, ArmyImage[4]);
    ConstructButton(5, ArmyImage[5]);


}
void PlayScene::ConstructButton(int id, std::string imageName) {
    ArmyButton* btn;
    // Button
    btn = new ArmyButton("play/floor.png", "play/dirt.png",
        Engine::Sprite(imageName, 175 + 120 * id, BlockSize * MapHeight + 10, 80, 80, 0, 0)
        , 170 + 120 * id,  BlockSize * MapHeight, 0, id);
    // Reference: Class Member Function Pointer and std::bind.
    btn->SetOnClickCallback(std::bind(&PlayScene::UIBtnClicked, this, id));
    UIGroup->AddNewControlObject(btn);
    // Button Label
    AddNewObject(UIArmyAmount[id] = new Engine::Label("x" + std::to_string(armyAmount[id]), "pirulen.ttf", 20.5, 230 + 120 * id, BlockSize * MapHeight + 110, 0, 0, 0, 255, 0.5, 0.5));
}

void PlayScene::UIBtnClicked(int id) {
    // might have chosen an army already, hence we need to delete first
	if (preview) {
		UIGroup->RemoveObject(preview->GetObjectIterator());
        preview = nullptr;
    }
    
    if (id == 0)
        preview = new ArcherArmy(0, 0);
    else if (id == 1)
        preview =new BombArmy(0, 0);
    else if (id == 2)
        preview = new KabiArmy(0, 0);
    else if (id == 5)
        preview = new Dio(0, 0);
    else if (id == 4)
        preview = new Dio_fight(0, 0);
    else if (id == 3)
        preview = new Cococute(0, 0);

	if (!preview)
		return;
    
    // set preview profile
	preview->Position = Engine::GameEngine::GetInstance().GetMousePosition();
	preview->Tint = al_map_rgba(255, 255, 255, 200);
	preview->Enabled = false;
    preview->isPreview = true;
	UIGroup->AddNewObject(preview);
	OnMouseMove(Engine::GameEngine::GetInstance().GetMousePosition().x, Engine::GameEngine::GetInstance().GetMousePosition().y);
}


bool PlayScene::CheckOccupied(int x, int y) {
    if (x < 0 || x >= MapWidth  || y < 0 || y >= MapHeight)
        return true;
    
    TileType tt = mapState[y][x];
    if (tt == TILE_WALL || tt == TILE_CANNON || tt==TILE_ARAARA) return true;
    
    if (x >= corners[0].x && x <= corners[1].x
        && y >= corners[0].y && y <= corners[2].y
        ) return true;
    
    return false;
}
bool PlayScene:: Checkworld(int x, int y) {
    if (x < 0 || x >= MapWidth || y < 0 || y >= MapHeight)
        return true;
    if (x >= corners[0].x && x <= corners[1].x
        && y >= corners[0].y && y <= corners[2].y
        ) return true;

    return false;
}
int PlayScene::GetArmyAmount(int id) {
    return armyAmount[id];
}

void PlayScene::SetArmyAmount(int id, int amount) {
    armyAmount[id] = amount;
}

int PlayScene::GetTotalArmyAmount() {
    return totalArmy;
}

void PlayScene::SetTotalArmyAmount(int total) {
    totalArmy = total;
}

void PlayScene::ClearMapState(int x, int y) {
    mapState[y][x] = TILE_FLOOR;
}
