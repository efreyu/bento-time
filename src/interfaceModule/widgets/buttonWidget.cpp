#include "buttonWidget.h"
#include "cocos-ext.h"

using namespace bt::interfaceModule;

buttonWidget::buttonWidget() {
    this->setName("buttonWidget");
    initWithProperties("widgets/" + this->getName());
    label = dynamic_cast<cocos2d::Label*>(findNode("btnText"));
    removeJsonData();
}

void buttonWidget::updateSettings() {
    auto settings = getSettingsData();
    if (settings.IsObject() && settings.HasMember("text") && settings["text"].IsString()) {
        setText(settings["text"].GetString());
    }
}

void buttonWidget::setText(const std::string& value) {
    label->setString(value);
}
