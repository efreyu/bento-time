#include "menuScene.h"
#include "generic/coreModule/scenes/scenesFactoryInstance.h"
#include "interfaceModule/widgets/buttonWidget.h"

using namespace bt::sceneModule;
using namespace cocos2d;

menuScene::menuScene() {
    this->setName("menuScene");
    initWithProperties("scenes/" + this->getName());
}

std::deque<nodeTasks> menuScene::getTasks() {
    std::deque<nodeTasks> result;
    result.emplace_back([this]() {
        if (auto btn = dynamic_cast<bt::interfaceModule::buttonWidget*>(findNode("btn"))) {
            btn->initListener();
            btn->setOnTouchEnded([]() {
                GET_SCENES_FACTORY().runScene("gameScene");
            });
        }
        return eTasksStatus::STATUS_OK;
    });
    return result;
}
