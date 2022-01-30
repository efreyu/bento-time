#include "gameScene.h"
#include "gameplayModule/gameBoard.h"
#include "generic/debugModule/logManager.h"
#include "interfaceModule/widgets/controllerWidget.h"

using namespace bt::sceneModule;

gameScene::gameScene() {
    this->setName("gameScene");
    initWithProperties("scenes/" + this->getName());
}

std::deque<nodeTasks> gameScene::getTasks() {
    std::deque<nodeTasks> result;
    result.emplace_back([this]() {
        if (auto panelHolder = findNode("panelHolder")) {
            auto board = new gameplayModule::gameBoard();
            panelHolder->addChild(board, -1);
            // todo this is fake value, I will change this after testing
            board->loadLevel(10001);
        } else {
            LOG_ERROR("Can't find 'topPanel' node!");
        }
        return eTasksStatus::STATUS_OK;
    });
    result.emplace_back([this]() {
        if (auto controllerNode = dynamic_cast<interfaceModule::controllerWidget*>(findNode("controller"))) {
            controllerNode->init();
        } else {
            LOG_ERROR("Can't find 'controller' node!");
        }
        return eTasksStatus::STATUS_OK;
    });
    return result;
}
