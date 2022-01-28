#include "menuButton.h"

using namespace bt::interfaceModule;

menuButton::menuButton() {
    this->setName("menuButton");
    initWithProperties("widgets/" + this->getName());
}

menuButton::~menuButton() {}
