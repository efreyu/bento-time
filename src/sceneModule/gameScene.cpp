#include "gameScene.h"
#include "gameplayModule/gameBoard.h"
#include "generic/debugModule/logManager.h"
#include "interfaceModule/widgets/controllerStickWidget.h"
#include "interfaceModule/widgets/controllerButtonWidget.h"

using namespace bt::sceneModule;

gameScene::gameScene() {
    this->setName("gameScene");
    initWithProperties("scenes/" + this->getName());
    if (!hasPropertyObject("settings"))
        return;
    const auto& json = getPropertyObject("settings");
    auto color = cocos2d::Color3B::BLACK;
    if (json.HasMember("bgColor") && json["bgColor"].IsArray()) {
        color.r = static_cast<uint8_t>(json["bgColor"][0u].GetInt());
        color.g = static_cast<uint8_t>(json["bgColor"][1u].GetInt());
        color.b = static_cast<uint8_t>(json["bgColor"][2u].GetInt());
    }
    initLayerColor(color);
    if (json.HasMember("fadeTransitionTime") && json["fadeTransitionTime"].IsNumber()) {
        fadeTransitionTime = json["fadeTransitionTime"].GetFloat();
    }
}

void gameScene::onSceneLoading() {
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
    auto levelId = 10001;
    if (sceneData.count("levelId")) {
        levelId = sceneData["levelId"].asInt();
    }
    board->loadLevel(levelId);
    controllerNode->init();
    buttonA->initListener();
    buttonB->initListener();
    buttonB->setOnTouchEnded([](){
        GET_SCENES_FACTORY().runScene("menuScene");
    });
    board->attachController(controllerNode->getEmitter(), buttonA);
}
