#include "customNodeTypes.h"
#include "generic/coreModule/nodes/nodeFactory.h"
#include "generic/coreModule/scenes/scenesFactoryInstance.h"
#include "interfaceModule/widgets/buttonWidget.h"
#include "interfaceModule/widgets/controllerStickWidget.h"
#include "interfaceModule/widgets/controllerButtonWidget.h"
#include "interfaceModule/widgets/menuButton.h"
#include "interfaceModule/widgets/rollbackButton.h"


void bt::interfaceModule::registerAllCustomNodes() {
    GET_NODE_FACTORY().registerCustomNodeType("buttonWidget", []() { return new buttonWidget(); });
    GET_NODE_FACTORY().registerCustomNodeType("menuButton", []() { return new menuButton(); });
    GET_NODE_FACTORY().registerCustomNodeType("rollbackButton", []() { return new rollbackButton(); });
    GET_NODE_FACTORY().registerCustomNodeType("controllerStickWidget", []() { return new controllerStickWidget(); });
    GET_NODE_FACTORY().registerCustomNodeType("controllerButtonWidget", []() { return new controllerButtonWidget(); });

}
