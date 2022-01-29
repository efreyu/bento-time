#include "gameScene.h"
#include "gameplayModule/gameBoard.h"

using namespace bt::sceneModule;

gameScene::gameScene() {
    this->setName("gameScene");
    initWithProperties("scenes/" + this->getName());
}

std::deque<nodeTasks> gameScene::getTasks() {
    std::deque<nodeTasks> result;
    result.emplace_back([this]() {
        auto board = new gameplayModule::gameBoard();
        addChild(board);
        // todo this is fake value, I will change this after testing
        board->loadLevel(10001);
        return eTasksStatus::STATUS_OK;
    });
    return result;
}
