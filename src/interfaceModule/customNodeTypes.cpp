#include "customNodeTypes.h"
#include "generic/coreModule/nodes/nodeFactory.h"
#include "generic/coreModule/scenes/scenesFactoryInstance.h"
#include "interfaceModule/widgets/buttonWidget.h"
#include "interfaceModule/widgets/controllerWidget.h"
#include "interfaceModule/widgets/menuButton.h"
#include "interfaceModule/widgets/rollbackButton.h"

// all windows
#include "interfaceModule/windows/notifyWindow.h"

void bt::interfaceModule::registerAllCustomNodes() {
    GET_NODE_FACTORY().registerCustomNodeType("buttonWidget", []() { return new buttonWidget(); });
    GET_NODE_FACTORY().registerCustomNodeType("menuButton", []() { return new menuButton(); });
    GET_NODE_FACTORY().registerCustomNodeType("rollbackButton", []() { return new rollbackButton(); });
    GET_NODE_FACTORY().registerCustomNodeType("controllerWidget", []() { return new controllerWidget(); });

    // register all windows
//    GET_CURRENT_SCENE()->getWindowSystem()->registerWindow("notifyWindow", []() { return new notifyWindow(); });
}
