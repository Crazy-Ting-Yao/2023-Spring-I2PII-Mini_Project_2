#include <allegro5/allegro.h>
#include <algorithm>
#include <cmath>
#include <fstream>
#include <functional>
#include <vector>
#include <queue>
#include <string>
#include <memory>

#include "include/AudioHelper.hpp"
#include "include/Effect/DirtyEffect.hpp"
#include "include/Enemy/Enemy.hpp"
#include "include/GameEngine.hpp"
#include "include/Group.hpp"
#include "include/IObject.hpp"
#include "include/Image.hpp"
#include "include/Label.hpp"
// Turret
#include "include/Turret/PlugGunTurret.hpp"
#include "include/Turret/MachineGun.hpp"
#include "include/Turret/DoubleMachineGun.hpp"
#include "include/Turret/Plane.hpp"
#include "include/Turret/OrbitTurret.hpp"
#include "include/Shovel.hpp"
#include "include/Shifter.hpp"
// Enemy
#include "include/Enemy/RedNormalEnemy.hpp"
#include "include/Scene/PlayScene.hpp"
#include "include/Resources.hpp"
#include "include/Sprite.hpp"
#include "include/Turret/Turret.hpp"
#include "include/Turret/TurretButton.hpp"
#include "include/ToolsButton.hpp"
#include "include/LOG.hpp"
#include "include/Enemy/DiceEnemy.hpp"
#include "include/Enemy/DiceTwoEnemy.hpp"
#include "include/Enemy/DiceThreeEnemy.hpp"
#include "include/Enemy/DiceFourEnemy.hpp"
#include "include/Enemy/DiceFiveEnemy.hpp"
#include "include/Enemy/DiceSixEnemy.hpp"

bool PlayScene::DebugMode = false;
const std::vector<Engine::Point> PlayScene::directions = { Engine::Point(-1, 0), Engine::Point(0, -1), Engine::Point(1, 0), Engine::Point(0, 1) };
const int PlayScene::MapWidth = 20, PlayScene::MapHeight = 13;
const int PlayScene::BlockSize = 64;
const float PlayScene::DangerTime = 7.61;
const Engine::Point PlayScene::SpawnGridPoint = Engine::Point(-1, 0);
const Engine::Point PlayScene::EndGridPoint = Engine::Point(MapWidth, MapHeight - 1);
// TODO 5 (2/3): Set the cheat code correctly.
const std::vector<int> PlayScene::code = { ALLEGRO_KEY_UP , ALLEGRO_KEY_DOWN, ALLEGRO_KEY_LEFT, ALLEGRO_KEY_RIGHT, ALLEGRO_KEY_LEFT, ALLEGRO_KEY_RIGHT, ALLEGRO_KEY_ENTER };
Engine::Point PlayScene::GetClientSize() {
    return Engine::Point(MapWidth * BlockSize, MapHeight * BlockSize);
}
void PlayScene::Initialize() {
    // TODO 6 (1/2): There's a bug in this file, which crashes the game when you win. Try to find it.
    // TODO 6 (2/2): There's a bug in this file, which doesn't update the player's life correctly when getting the first attack. Try to find it.
    mapState.clear();
    keyStrokes.clear();
    ticks = 0;
    deathCountDown = -1;
    lives = 10;
    money = 1500;
    SpeedMult = 1;
    // Add groups from bottom to top.
    AddNewObject(TileMapGroup = new Group());
    AddNewObject(GroundEffectGroup = new Group());
    AddNewObject(DebugIndicatorGroup = new Group());
    AddNewObject(TowerGroup = new Group());
    AddNewObject(EnemyGroup = new Group());
    AddNewObject(BulletGroup = new Group());
    AddNewObject(EffectGroup = new Group());
    // Should support buttons.
    AddNewControlObject(UIGroup = new Group());
    ReadMap();
    ReadEnemyWave();
    mapDistance = CalculateBFSDistance();
    ConstructUI();
    imgTarget = new Engine::Image("play/target.png", 0, 0);
    imgTarget->Visible = false;
    preview.mode = 0;
    UIGroup->AddNewObject(imgTarget);
    // Preload Lose Scene
    deathBGMInstance = Engine::Resources::GetInstance().GetSampleInstance("astronomia.ogg");
    Engine::Resources::GetInstance().GetBitmap("lose/benjamin-happy.png");
    // Start BGM.
    // bgmId = AudioHelper::PlayBGM("play.ogg");
    if (!mute)
        bgmInstance = AudioHelper::PlaySample("play.ogg", true, AudioHelper::BGMVolume);
    else
        bgmInstance = AudioHelper::PlaySample("play.ogg", true, 0.0);
}
void PlayScene::Terminate() {
    AudioHelper::StopBGM(bgmId);
    AudioHelper::StopSample(deathBGMInstance);
    AudioHelper::StopSample(bgmInstance);
    deathBGMInstance = std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE>();
    IScene::Terminate();
}
void PlayScene::Update(float deltaTime) {
    // If we use deltaTime directly, then we might have Bullet-through-paper problem.
    // Reference: Bullet-Through-Paper
    if (SpeedMult == 0)
        deathCountDown = -1;
    else if (deathCountDown != -1)
        SpeedMult = 1;
    // Calculate danger zone.
    std::vector<float> reachEndTimes;
    for (auto& it : EnemyGroup->GetObjects()) {
        reachEndTimes.push_back(dynamic_cast<Enemy*>(it)->reachEndTime);
    }
    // Can use Heap / Priority-Queue instead. But since we won't have too many enemies, sorting is fast enough.
    std::sort(reachEndTimes.begin(), reachEndTimes.end());
    float newDeathCountDown = -1;
    int danger = lives;
    for (auto& it : reachEndTimes) {
        if (it <= DangerTime) {
            danger--;
            if (danger <= 0) {
                // Death Countdown
                float pos = DangerTime - it;
                if (it > deathCountDown) {
                    // Restart Death Count Down BGM.
                    AudioHelper::StopSample(deathBGMInstance);
                    if (SpeedMult != 0)
                        deathBGMInstance = AudioHelper::PlaySample("astronomia.ogg", false, AudioHelper::BGMVolume, pos);
                }
                float alpha = pos / DangerTime;
                alpha = std::max(0, std::min(255, static_cast<int>(alpha * alpha * 255)));
                dangerIndicator->Tint = al_map_rgba(255, 255, 255, alpha);
                newDeathCountDown = it;
                break;
            }
        }
    }
    deathCountDown = newDeathCountDown;
    if (SpeedMult == 0)
        AudioHelper::StopSample(deathBGMInstance);
    if (deathCountDown == -1 && lives > 0) {
        AudioHelper::StopSample(deathBGMInstance);
        dangerIndicator->Tint.a = 0;
    }
    if (SpeedMult == 0)
        deathCountDown = -1;
    for (int i = 0; i < SpeedMult; i++) {
        IScene::Update(deltaTime);
        // Check if we should create new enemy.
        ticks += deltaTime;
        if (enemyWaveData.empty()) {
            if (EnemyGroup->GetObjects().empty()) {
                Engine::GameEngine::GetInstance().ChangeScene("win");
            }
            continue;
        }
        auto current = enemyWaveData.front();
        if (ticks < current.second)
            continue;
        ticks -= current.second;
        enemyWaveData.pop_front();
        const Engine::Point SpawnCoordinate = Engine::Point(SpawnGridPoint.x * BlockSize + BlockSize / 2, SpawnGridPoint.y * BlockSize + BlockSize / 2);
        Enemy* enemy;
        switch (current.first) {
            case 1:
                EnemyGroup->AddNewObject(enemy = new RedNormalEnemy(SpawnCoordinate.x, SpawnCoordinate.y));
                break;
                // TODO 2 (2/3): You need to modify 'resources/enemy1.txt', or 'resources/enemy2.txt' to spawn the new enemy.
            case 2:
                EnemyGroup->AddNewObject(enemy = new DiceEnemy(SpawnCoordinate.x, SpawnCoordinate.y));
                break;
            case 3:
                EnemyGroup->AddNewObject(enemy = new DiceTwoEnemy(SpawnCoordinate.x, SpawnCoordinate.y));
                break;
            case 4:
                EnemyGroup->AddNewObject(enemy = new DiceThreeEnemy(SpawnCoordinate.x, SpawnCoordinate.y));
                break;
            case 5:
                EnemyGroup->AddNewObject(enemy = new DiceFourEnemy(SpawnCoordinate.x, SpawnCoordinate.y));
                break;
            case 6:
                EnemyGroup->AddNewObject(enemy = new DiceFiveEnemy(SpawnCoordinate.x, SpawnCoordinate.y));
                break;
            case 7:
                EnemyGroup->AddNewObject(enemy = new DiceSixEnemy(SpawnCoordinate.x, SpawnCoordinate.y));
                break;
                // The format is "[EnemyId] [TimeDelay] [Repeat]".
                // TODO 2 (3/3): Enable the creation of the new enemy.
            default:
                continue;
        }
        enemy->UpdatePath(mapDistance);
        // Compensate the time lost.
        enemy->Update(ticks);
    }
    if (preview.mode == 1 || preview.mode == 4) {
        preview.turret_preview->Position = Engine::GameEngine::GetInstance().GetMousePosition();
        // To keep responding when paused.
        preview.turret_preview->Update(deltaTime);
    }
    else if(preview.mode == 2) {
        preview.shovel_preview->Position = Engine::GameEngine::GetInstance().GetMousePosition();
        // To keep responding when paused.
        preview.shovel_preview->Update(deltaTime);
    }
    else if(preview.mode == 3){
        preview.shifter_preview->Position = Engine::GameEngine::GetInstance().GetMousePosition();
        // To keep responding when paused.
        preview.shifter_preview->Update(deltaTime);
    }
}
void PlayScene::Draw() const {
    IScene::Draw();
    if (DebugMode) {
        // Draw reverse BFS distance on all reachable blocks.
        for (int i = 0; i < MapHeight; i++) {
            for (int j = 0; j < MapWidth; j++) {
                if (mapDistance[i][j] != -1) {
                    // Not elegant nor efficient, but it's quite enough for debugging.
                    Engine::Label label(std::to_string(mapDistance[i][j]), "pirulen.ttf", 32, (j + 0.5) * BlockSize, (i + 0.5) * BlockSize);
                    label.Anchor = Engine::Point(0.5, 0.5);
                    label.Draw();
                }
            }
        }
    }
}
void PlayScene::OnMouseDown(int button, int mx, int my) {
    if ((button & 1) && !imgTarget->Visible && (preview.mode == 1 || preview.mode == 4)) {
        // Cancel turret construct.
        UIGroup->RemoveObject(preview.turret_preview->GetObjectIterator());
        preview.turret_preview = nullptr;
        preview.mode = 0;
    }
    else if((button & 1) && !imgTarget->Visible && preview.mode == 2) {
        // Cancel turret construct.
        UIGroup->RemoveObject(preview.shovel_preview->GetObjectIterator());
        preview.shovel_preview = nullptr;
        preview.mode = 0;
    }
    else if((button & 1) && !imgTarget->Visible && preview.mode == 3) {
        // Cancel turret construct.
        UIGroup->RemoveObject(preview.shifter_preview->GetObjectIterator());
        preview.shifter_preview = nullptr;
        preview.mode = 0;
    }
    IScene::OnMouseDown(button, mx, my);
}
void PlayScene::OnMouseMove(int mx, int my) {
    IScene::OnMouseMove(mx, my);
    const int x = mx / BlockSize;
    const int y = my / BlockSize;
    if (!preview.mode || x < 0 || x >= MapWidth || y < 0 || y >= MapHeight) {
        imgTarget->Visible = false;
        return;
    };
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
    if (button & 1) {
        if (!preview.mode)
            return;
        else if(preview.mode == 1 || preview.mode == 4){
            if (mapState[y][x] != TILE_OCCUPIED) {
                // Check if valid.
                if (!CheckSpaceValid(x, y)) {
                    Engine::Sprite* sprite;
                    GroundEffectGroup->AddNewObject(sprite = new DirtyEffect("play/target-invalid.png", 1, x * BlockSize + BlockSize / 2, y * BlockSize + BlockSize / 2));
                    sprite->Rotation = 0;
                    return;
                }
                // Purchase.
                if(preview.mode == 1) EarnMoney(-preview.turret_preview->GetPrice());
                // Remove Preview.
                preview.turret_preview->GetObjectIterator()->first = false;
                UIGroup->RemoveObject(preview.turret_preview->GetObjectIterator());
                // Construct real turret.
                preview.turret_preview->Position.x = x * BlockSize + BlockSize / 2;
                preview.turret_preview->Position.y = y * BlockSize + BlockSize / 2;
                preview.turret_preview->Enabled = true;
                preview.turret_preview->Preview = false;
                preview.turret_preview->Tint = al_map_rgba(255, 255, 255, 255);
                TowerGroup->AddNewObject(preview.turret_preview);
                Turrets[y][x] = preview.turret_preview;
                // To keep responding when paused.
                preview.turret_preview->Update(0);
                // Remove Preview.
                preview.turret_preview = nullptr;
                preview.mode = 0;
                // Update mapState
                mapState[y][x] = TILE_OCCUPIED;
                OnMouseMove(mx, my);
            }
            else if (preview.turret_preview->GetId() == 1 && Turrets[y][x]->GetId() == 1) {
                TowerGroup->RemoveObject(Turrets[y][x]->GetObjectIterator());
                // Purchase.
                if(preview.mode==1) EarnMoney(-preview.turret_preview->GetPrice());
                // Remove Preview.
                preview.turret_preview->GetObjectIterator()->first = false;
                UIGroup->RemoveObject(preview.turret_preview->GetObjectIterator());
                // Construct real turret.
                preview.turret_preview->Position.x = x * BlockSize + BlockSize / 2;
                preview.turret_preview->Position.y = y * BlockSize + BlockSize / 2;
                preview.turret_preview->Enabled = true;
                preview.turret_preview->Preview = false;
                preview.turret_preview->Tint = al_map_rgba(255, 255, 255, 255);
                preview.turret_preview = new DoubleMachineGun(x * BlockSize + BlockSize / 2, y * BlockSize + BlockSize / 2);
                TowerGroup->AddNewObject(preview.turret_preview);
                Turrets[y][x] = preview.turret_preview;
                // To keep responding when paused.
                preview.turret_preview->Update(0);
                // Remove Preview.
                preview.turret_preview = nullptr;
                preview.mode = 0;
                // Update mapState
                mapState[y][x] = TILE_OCCUPIED;
                OnMouseMove(mx, my);
            }
        }
        else if(preview.mode == 2){
            if (mapState[y][x] != TILE_OCCUPIED || !CheckSpaceValid(x, y)) {
                Engine::Sprite* sprite;
                GroundEffectGroup->AddNewObject(sprite = new DirtyEffect("play/target-invalid.png", 1, x * BlockSize + BlockSize / 2, y * BlockSize + BlockSize / 2));
                sprite->Rotation = 0;
                return;
            }
            else if (mapState[y][x] == TILE_OCCUPIED) {
                // Check if valid.
                // Purchase.
                EarnMoney(Turrets[y][x]->GetPrice()/2);
                // Remove Preview.
                preview.shovel_preview->GetObjectIterator()->first = false;
                UIGroup->RemoveObject(preview.shovel_preview->GetObjectIterator());
                // Construct real turret.
                TowerGroup->RemoveObject(Turrets[y][x]->GetObjectIterator());
                Turrets[y][x] = nullptr;
                // To keep responding when paused.
                // Remove Preview.
                preview.shovel_preview = nullptr;
                preview.mode = 0;
                // Update mapState
                mapState[y][x] = TILE_FLOOR;
                OnMouseMove(mx, my);
            }
        }
        else if(preview.mode == 3){
            if (mapState[y][x] != TILE_OCCUPIED || !CheckSpaceValid(x, y)) {
                Engine::Sprite* sprite;
                GroundEffectGroup->AddNewObject(sprite = new DirtyEffect("play/target-invalid.png", 1, x * BlockSize + BlockSize / 2, y * BlockSize + BlockSize / 2));
                sprite->Rotation = 0;
                return;
            }
            else if (mapState[y][x] == TILE_OCCUPIED) {
                // Check if valid.
                // Purchase.
                // Remove Preview.
                preview.shifter_preview->GetObjectIterator()->first = false;
                // Construct real turret.
                UIBtnClicked(Turrets[y][x]->GetId()+10);
                TowerGroup->RemoveObject(Turrets[y][x]->GetObjectIterator());
                Turrets[y][x] = nullptr;
                // To keep responding when paused.
                // Remove Preview.
                preview.shifter_preview = nullptr;
                // Update mapState
                mapState[y][x] = TILE_FLOOR;
                OnMouseMove(mx, my);

            }
        }
    }
}
void PlayScene::OnKeyDown(int keyCode) {
    IScene::OnKeyDown(keyCode);
    if (keyCode == ALLEGRO_KEY_TAB) {
        // TODO 5 (1/3): Set Tab as a code to active / de-active the debug mode.
        DebugMode = !DebugMode;
    }
    else {
        keyStrokes.push_back(keyCode);
        if (keyStrokes.size() > code.size())
            keyStrokes.pop_front();
        // TODO 5 (3/3): Check whether the input sequence corresponds to the code. If so, active a plane and earn 10000 money.
        auto it = keyStrokes.begin();
        bool flag = true;
        for(int i = 0; i < code.size(); i++) {
            if (*it != code[i])
                flag = false;
            else it++;
        }
        if (flag){
            EffectGroup->AddNewObject(new Plane());
            money += 10000;
        }
        // Active a plane : EffectGroup->AddNewObject(new Plane());
        // Earn money : money += 10000;
    }
    if (keyCode == ALLEGRO_KEY_Q) {
        // Hotkey for PlugGunTurret.
        UIBtnClicked(0);
    }
        // TODO 3 (5/5): Make the W key to create the new turret.
    else if (keyCode == ALLEGRO_KEY_W) {
        UIBtnClicked(1);
        // Hotkey for new turret.
    }
    else if (keyCode == ALLEGRO_KEY_E) {
        UIBtnClicked(2);
        // Hotkey for new turret.
    }
    else if (keyCode == ALLEGRO_KEY_A) {
        UIBtnClicked(7);
        // Hotkey for new turret.
    }
    else if (keyCode == ALLEGRO_KEY_S) {
        UIBtnClicked(8);
        // Hotkey for new turret.
    }
    else if (keyCode >= ALLEGRO_KEY_0 && keyCode <= ALLEGRO_KEY_9) {
        // Hotkey for Speed up.
        SpeedMult = keyCode - ALLEGRO_KEY_0;
    }
    else if (keyCode == ALLEGRO_KEY_M) {
        // Hotkey for mute / unmute.
        if (mute)
            AudioHelper::ChangeSampleVolume(bgmInstance, AudioHelper::BGMVolume);
        else
            AudioHelper::ChangeSampleVolume(bgmInstance, 0.0);
        mute = !mute;
    }
}
void PlayScene::Hit() {
    UILives->Text = std::string("Life ") + std::to_string(--lives);
    if (lives <= 0) {
        Engine::GameEngine::GetInstance().ChangeScene("lose");
    }
}
int PlayScene::GetMoney() const {
    return money;
}
void PlayScene::EarnMoney(int money) {
    this->money += money;
    UIMoney->Text = std::string("$") + std::to_string(this->money);
}
void PlayScene::ReadMap() {
    std::string filename = std::string("resources/map") + std::to_string(MapId) + ".txt";
    // Read map file.
    char c;
    std::vector<bool> mapData;
    std::ifstream fin(filename);
    while (fin >> c) {
        switch (c) {
            case '0': mapData.push_back(false); break;
            case '1': mapData.push_back(true); break;
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
    Turrets = std::vector<std::vector<Turret*>>(MapHeight, std::vector<Turret*>(MapWidth));
    for (int i = 0; i < MapHeight; i++) {
        for (int j = 0; j < MapWidth; j++) {
            const int num = mapData[i * MapWidth + j];
            mapState[i][j] = num ? TILE_FLOOR : TILE_DIRT;
            if (num)
                TileMapGroup->AddNewObject(new Engine::Image("play/floor.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
            else
                TileMapGroup->AddNewObject(new Engine::Image("play/dirt.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
        }
    }
}
void PlayScene::ReadEnemyWave() {
    std::string filename = std::string("resources/enemy") + std::to_string(MapId) + ".txt";
    // Read enemy file.
    float type, wait, repeat;
    enemyWaveData.clear();
    std::ifstream fin(filename);
    while (fin >> type && fin >> wait && fin >> repeat) {
        for (int i = 0; i < repeat; i++)
            enemyWaveData.emplace_back(type, wait);
    }
    fin.close();
}
void PlayScene::ConstructUI() {
    // Background
    UIGroup->AddNewObject(new Engine::Image("play/sand.png", 1280, 0, 320, 832));
    // Text
    UIGroup->AddNewObject(new Engine::Label(std::string("Stage ") + std::to_string(MapId), "pirulen.ttf", 32, 1294, 0));
    UIGroup->AddNewObject(UIMoney = new Engine::Label(std::string("$") + std::to_string(money), "pirulen.ttf", 24, 1294, 48));
    UIGroup->AddNewObject(UILives = new Engine::Label(std::string("Life ") + std::to_string(lives), "pirulen.ttf", 24, 1294, 88));
    // Buttons
    ConstructTurretButton(0, "play/turret-6.png", PlugGunTurret::Price);
    ConstructTurretButton(1, "play/turret-1.png", MachineGun::Price);
    ConstructTurretButton(2, "play/turret-4.png", OrbitTurret::Price);
    ConstructToolsButton(7, "play/shovel.png");
    ConstructToolsButton(8, "play/shifter.png");

    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int shift = 135 + 25;
    dangerIndicator = new Engine::Sprite("play/benjamin.png", w - shift, h - shift);
    dangerIndicator->Tint.a = 0;
    UIGroup->AddNewObject(dangerIndicator);
}

void PlayScene::ConstructTurretButton(int id, std::string sprite, int price) {
    TurretButton* btn = new TurretButton("play/floor.png", "play/dirt.png",
                               Engine::Sprite("play/tower-base.png", 1294 + id * 76, 136, 0, 0, 0, 0),
                               Engine::Sprite(sprite, 1294 + id * 76, 136 - 8, 0, 0, 0, 0)
                , 1294 + id * 76, 136, price);
        // Reference: Class Member Function Pointer and std::bind.
    btn->SetOnClickCallback(std::bind(&PlayScene::UIBtnClicked, this, id));
    UIGroup->AddNewControlObject(btn);
}

void PlayScene::ConstructToolsButton(int id, std::string sprite) {
    ToolsButton* btn = new ToolsButton("play/floor.png", "play/dirt.png",
                               Engine::Sprite(sprite, 1294 + (id-7) * 76, 136 + 80 , 0, 0, 0, 0)
                , 1294 + (id-7) * 76, 136 + 80);
        // Reference: Class Member Function Pointer and std::bind.
    btn->SetOnClickCallback(std::bind(&PlayScene::UIBtnClicked, this, id));
    UIGroup->AddNewControlObject(btn);
}

void PlayScene::UIBtnClicked(int id) {
    if (preview.mode == 1 || preview.mode == 4) {
        UIGroup->RemoveObject(preview.turret_preview->GetObjectIterator());
        preview.turret_preview = nullptr;
    }
    else if(preview.mode == 2) {
        UIGroup->RemoveObject(preview.shovel_preview->GetObjectIterator());
        preview.shovel_preview = nullptr;
    }
    else if(preview.mode == 3) {
        UIGroup->RemoveObject(preview.shifter_preview->GetObjectIterator());
        preview.shifter_preview = nullptr;
    }
    preview.mode = 0;
    if (id == 7){
        preview.shovel_preview = new Shovel(0, 0);
        preview.mode = 2;
        preview.shovel_preview->Position = Engine::GameEngine::GetInstance().GetMousePosition();
        preview.shovel_preview->Tint = al_map_rgba(255, 255, 255, 200);
        preview.shovel_preview->Enabled = false;
        preview.shovel_preview->Preview = true;
        UIGroup->AddNewObject(preview.shovel_preview);
    }
    else if(id == 8) {
        preview.shifter_preview = new Shifter(0, 0);
        preview.mode = 3;
        preview.shifter_preview->Position = Engine::GameEngine::GetInstance().GetMousePosition();
        preview.shifter_preview->Tint = al_map_rgba(255, 255, 255, 200);
        preview.shifter_preview->Enabled = false;
        preview.shifter_preview->Preview = true;
        UIGroup->AddNewObject(preview.shifter_preview);
    }
    else{
        if (id == 0 && money >= PlugGunTurret::Price){
            preview.turret_preview = new PlugGunTurret(0, 0);
            preview.mode = 1;
        }
        else if (id == 1 && money >= MachineGun::Price){
            preview.turret_preview = new MachineGun(0, 0);
            preview.mode = 1;
        }
        else if (id == 2 && money >= OrbitTurret::Price){
            preview.turret_preview = new OrbitTurret(0, 0);
            preview.mode = 1;
        }
        else if (id == 10){
            preview.turret_preview = new PlugGunTurret(0, 0);
            preview.mode = 4;
        }
        else if (id == 11){
            preview.turret_preview = new MachineGun(0, 0);
            preview.mode = 4;
        }
        else if (id == 12){
            preview.turret_preview = new OrbitTurret(0, 0);
            preview.mode = 4;
        }
        else if(id == 19){
            preview.turret_preview = new DoubleMachineGun(0, 0);
            preview.mode = 4;
        }
        if(!preview.mode)
            return;
        preview.turret_preview->Position = Engine::GameEngine::GetInstance().GetMousePosition();
        preview.turret_preview->Tint = al_map_rgba(255, 255, 255, 200);
        preview.turret_preview->Enabled = false;
        preview.turret_preview->Preview = true;
        UIGroup->AddNewObject(preview.turret_preview);
    }
    if (!preview.mode){
        return;
    }
    OnMouseMove(Engine::GameEngine::GetInstance().GetMousePosition().x, Engine::GameEngine::GetInstance().GetMousePosition().y);
}

bool PlayScene::CheckSpaceValid(int x, int y) {
    if (x < 0 || x >= MapWidth || y < 0 || y >= MapHeight)
        return false;
    auto map00 = mapState[y][x];
    mapState[y][x] = TILE_OCCUPIED;
    std::vector<std::vector<int>> map = CalculateBFSDistance();
    mapState[y][x] = map00;
    if (map[0][0] == -1)
        return false;
    for (auto& it : EnemyGroup->GetObjects()) {
        Engine::Point pnt;
        pnt.x = floor(it->Position.x / BlockSize);
        pnt.y = floor(it->Position.y / BlockSize);
        if (pnt.x < 0) pnt.x = 0;
        if (pnt.x >= MapWidth) pnt.x = MapWidth - 1;
        if (pnt.y < 0) pnt.y = 0;
        if (pnt.y >= MapHeight) pnt.y = MapHeight - 1;
        if (map[pnt.y][pnt.x] == -1)
            return false;
    }
    // All enemy have path to exit.
    mapState[y][x] = TILE_OCCUPIED;
    mapDistance = map;
    for (auto& it : EnemyGroup->GetObjects())
        dynamic_cast<Enemy*>(it)->UpdatePath(mapDistance);
    return true;
}
std::vector<std::vector<int>> PlayScene::CalculateBFSDistance() {
    // Reverse BFS to find path.
    std::vector<std::vector<int>> map(MapHeight, std::vector<int>(std::vector<int>(MapWidth, -1)));
    std::queue<Engine::Point> que;
    // Push end point.
    // BFS from end point.
    if (mapState[MapHeight - 1][MapWidth - 1] != TILE_DIRT)
        return map;
    que.push(Engine::Point(MapWidth - 1, MapHeight - 1));
    map[MapHeight - 1][MapWidth - 1] = 0;
    while (!que.empty()) {
        Engine::Point p = que.front();
        que.pop();
        for (auto &c : directions) {
            int x = p.x + c.x;
            int y = p.y + c.y;
            if (x < 0 || x >= MapWidth || y < 0 || y >= MapHeight ||
                map[y][x] != -1 || mapState[y][x] != TILE_DIRT) {
                continue;
            } else {
                map[y][x] = map[p.y][p.x] + 1;
                que.push(Engine::Point(x, y));
            }
        }
    }
    return map;
}