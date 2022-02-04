#include "gameScene.h"
#include "gameplayModule/gameBoard.h"
#include "generic/debugModule/logManager.h"
#include "interfaceModule/widgets/controllerStickWidget.h"
#include "interfaceModule/widgets/controllerButtonWidget.h"

using namespace bt::sceneModule;

gameScene::gameScene() {
    this->setName("gameScene");
    initWithProperties("scenes/" + this->getName());
}

void gameScene::onSceneLoading() {
    sceneInterface::onSceneLoading();
    auto panelHolder = findNode("panelHolder");
    auto controllerNode = dynamic_cast<interfaceModule::controllerStickWidget*>(findNode("controller"));
    auto buttonA = dynamic_cast<interfaceModule::controllerButtonWidget*>(findNode("buttonA"));
    auto buttonB = dynamic_cast<interfaceModule::controllerButtonWidget*>(findNode("buttonB"));
    if (!panelHolder || !controllerNode
        || !buttonA || !buttonB) {
        LOG_ERROR("Problems with loading nodes.");
        return;
    }
    auto board = new gameplayModule::gameBoard();
    panelHolder->addChild(board, -1);
    // todo this is fake value, I will change this after testing
    board->loadLevel(10001);
    controllerNode->init();
    buttonA->initListener();
    buttonB->initListener();
    board->attachController(controllerNode->getEmitter(), buttonB);
}
