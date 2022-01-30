#include "controllerWidget.h"

using namespace bt::interfaceModule;

controllerWidget::controllerWidget() {
    this->setName("controllerWidget");
    initWithProperties("widgets/" + this->getName());
    removeJsonData();
}
