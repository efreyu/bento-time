#include "controllerButtonWidget.h"
#include <generic/audioModule/audioEngineInstance.h>
#include <map>
#include <string>

using namespace bt::interfaceModule;

const std::map<std::string, eControllerButtonType> buttonTypes = {
    {"typeA", eControllerButtonType::TYPE_A},
    {"typeB", eControllerButtonType::TYPE_B}
};

const std::map<std::string, eControllerIconType> iconTypes = {
    {"menu", eControllerIconType::MENU},
    {"replay", eControllerIconType::REPLAY}
};

controllerButtonWidget::controllerButtonWidget() {
    this->setName("controllerButtonWidget");
    initWithProperties("widgets/" + this->getName());
    btnNode = dynamic_cast<generic::coreModule::asepriteNode*>(findNode("button"));
    iconNode = dynamic_cast<generic::coreModule::asepriteNode*>(findNode("icon"));
    setChangeColorByClick(false);
}

void controllerButtonWidget::updateSettings() {
    auto type = eControllerButtonType::TYPE_A;
    auto icon = eControllerIconType::MENU;
    auto settings = getSettingsData();
    if (!settings.IsObject()) {
        updateButton(type, icon);
        return;
    }
    if (settings.HasMember("buttonType") && settings["buttonType"].IsString() && buttonTypes.count(settings["buttonType"].GetString())){
        type = buttonTypes.at(settings["buttonType"].GetString());
    }
    if (settings.HasMember("iconType") && settings["iconType"].IsString() && iconTypes.count(settings["iconType"].GetString())){
        icon = iconTypes.at(settings["iconType"].GetString());
    }
    std::string iconAnimation;
    if (settings.HasMember("iconAnimation") && settings["iconAnimation"].IsString()){
        iconAnimation = settings["iconAnimation"].GetString();
    }
    updateButton(type, icon, iconAnimation);
}

void controllerButtonWidget::updateButton(eControllerButtonType _buttonType, eControllerIconType iconType, const std::string& iconAnimation) {
    if (!btnNode)
        return;
    switch (_buttonType) {
    case eControllerButtonType::TYPE_A:
        loadProperty(btnNode, "typeA");
        break;
    case eControllerButtonType::TYPE_B:
        loadProperty(btnNode, "typeB");
        break;
    }
    buttonType = _buttonType;
    if (!iconNode)
        return;
    switch (iconType) {
    case eControllerIconType::MENU:
        loadProperty(iconNode, "iconReplay");
        break;
    case eControllerIconType::REPLAY:
        loadProperty(iconNode, "iconMenu");
        break;
    }
    if (!iconAnimation.empty()) {
        iconNode->setAnimation(iconAnimation);
    }
}

generic::coreModule::eventNode::eventTouchClb controllerButtonWidget::getOnTouchBegan() {
    btnNode->setAnimation("pressed");
    GET_AUDIO_ENGINE().play("ui.click");
    return eventNode::getOnTouchBegan();
}

generic::coreModule::eventNode::eventTouchClb controllerButtonWidget::getOnTouchEnded() {
    btnNode->setAnimation("default");
    return eventNode::getOnTouchEnded();
}
