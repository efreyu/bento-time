#include "rollbackButton.h"

using namespace bt::interfaceModule;

rollbackButton::rollbackButton() {
    this->setName("rollbackButton");
    initWithProperties("widgets/" + this->getName());
    setLabelText();
}

rollbackButton::~rollbackButton() {}

cocos2d::Label* rollbackButton::getLabel() {
    if (auto label = dynamic_cast<cocos2d::Label*>(findNode("notificationCounter")))
        return label;
    return nullptr;
}

void rollbackButton::setLabelText(const std::string& str) {
    if (auto sprite = findNode("notificationBg")) {
        sprite->setVisible(!str.empty());
    }
    if (auto label = getLabel()) {
        label->setString(str);
    }
}
