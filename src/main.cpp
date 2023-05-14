// [main.cpp]
// this template is provided for the 2D tower defense game.
// Program entry point.
// Returns program exit code.
#include "include/GameEngine.hpp"
#include "include/LOG.hpp"
#include "include/LoseScene.hpp"
#include "include/PlayScene.hpp"
#include "include/StartScene.hpp"
#include "include/StageSelectScene.hpp"
#include "include/WinScene.hpp"
#include "include/SettingScene.hpp"

int main(int argc, char **argv) {
    Engine::LOG::SetConfig(true);
    Engine::GameEngine& game = Engine::GameEngine::GetInstance();
    game.AddNewScene("stage-select", new StageSelectScene());
    game.AddNewScene("play", new PlayScene());
    game.AddNewScene("lose", new LoseScene());
    game.AddNewScene("win", new WinScene());
    game.AddNewScene("start-scene", new StartScene());
    game.AddNewScene("setting-scene", new SettingScene());
    game.Start("start-scene", 60, 1600, 832);
    return 0;
}
