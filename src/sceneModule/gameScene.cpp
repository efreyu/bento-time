#include "gameScene.h"
#include "gameplayModule/gameBoard.h"
#include "generic/debugModule/logManager.h"
#include "interfaceModule/widgets/controllerStickWidget.h"
#include "interfaceModule/widgets/controllerButtonWidget.h"

using namespace bt::sceneModule;

gameScene::gameScene() {
    this->setName("gameScene");
    initLayerColor(cocos2d::Color3B(114,101,181));
    initWithProperties("scenes/" + this->getName());
//    initLayerColor(color);
}

void gameScene::onSceneLoading() {
//    if (!hasPropertyObject("settings"))
//        return;
//    const auto& json = getPropertyObject("settings");
//    auto color = cocos2d::Color3B::BLACK;
//    if (json.HasMember("bgColor") && json["bgColor"].IsArray()) {
//        color.r = static_cast<uint8_t>(json["bgColor"][0u].GetInt());
//        color.g = static_cast<uint8_t>(json["bgColor"][1u].GetInt());
//        color.b = static_cast<uint8_t>(json["bgColor"][2u].GetInt());
//    }
//    GET_CURRENT_SCENE()->initLayerColor(color);
    sceneInterface::onSceneLoading();
    auto displayHolder = findNode("displayHolder");
    auto controllerNode = dynamic_cast<interfaceModule::controllerStickWidget*>(findNode("controller"));
    auto buttonA = dynamic_cast<interfaceModule::controllerButtonWidget*>(findNode("buttonA"));
    auto buttonB = dynamic_cast<interfaceModule::controllerButtonWidget*>(findNode("buttonB"));
    if (!displayHolder || !controllerNode
        || !buttonA || !buttonB) {
        LOG_ERROR("Problems with loading nodes.");
        return;
    }
    auto board = new gameplayModule::gameBoard();
    displayHolder->addChild(board, -1);
    // todo this is fake value, I will change this after testing
    board->loadLevel(10001);
    controllerNode->init();
    buttonA->initListener();
    buttonB->initListener();
    buttonA->setOnTouchBegan([](){
        GET_SCENES_FACTORY().runScene("menuScene");
    });
    board->attachController(controllerNode->getEmitter(), buttonB);
}
