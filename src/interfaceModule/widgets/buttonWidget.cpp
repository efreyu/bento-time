#include "buttonWidget.h"
#include "ui/CocosGUI.h"
#include "generic/utilityModule/convertUtility.h"

using namespace bt::interfaceModule;

buttonWidget::buttonWidget() {
    this->setName("buttonWidget");
    initWithProperties("widgets/" + this->getName());
    setSelect(false);
    initWidget();
}

void buttonWidget::setText(const std::string& value) {
    if (auto label = dynamic_cast<cocos2d::Label*>(findNode("label"))) {
        label->setString(value);
        label->init();
        auto size = label->getContentSize();
        setContentSize(size);
        if (auto select = dynamic_cast<cocos2d::ui::Scale9Sprite*>(findNode("select"))) {
            select->setContentSize({size.width + selectHorizontalOffset, size.height});
        }
    }
}

void buttonWidget::setDisabled() {
    if (auto label = dynamic_cast<cocos2d::Label*>(findNode("label"))) {
        loadProperty(label, "gray");
    }
}

void buttonWidget::setSelect(bool status) {
    if (auto select = dynamic_cast<cocos2d::ui::Scale9Sprite*>(findNode("select"))) {
        select->setVisible(status);
    }
}

void buttonWidget::initWidget() {
    if (!hasPropertyObject("settings"))
        return;
    const auto& json = getPropertyObject("settings");
    if (json.HasMember("selectHorizontalOffset") && json["selectHorizontalOffset"].IsNumber()) {
        selectHorizontalOffset = json["selectHorizontalOffset"].GetFloat();
    }
    float lightening = 1.f;
    float selectDuration = .1f;
    if (json.HasMember("lightening") && json["lightening"].IsNumber()) {
        lightening = json["lightening"].GetFloat();
    }
    if (json.HasMember("selectDuration") && json["selectDuration"].IsNumber()) {
        selectDuration = json["selectDuration"].GetFloat();
    }
    if (auto select = findNode("select")) {
        auto color = select->getColor();
        auto nextColor = generic::utilityModule::convertUtility::changeColorByPercent(color, lightening);
        auto tint = cocos2d::TintTo::create(selectDuration, nextColor);
        auto reverse = cocos2d::TintTo::create(selectDuration, color);
        auto seq = cocos2d::Sequence::create(tint, reverse, nullptr);
        select->runAction(cocos2d::RepeatForever::create(seq));
    }
}
