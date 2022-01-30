#ifndef BENTO_TIME_CONTROLLERWIDGET_H
#define BENTO_TIME_CONTROLLERWIDGET_H

#include "cocos2d.h"
#include "generic/coreModule/nodes/nodeProperties.h"
#include "generic/coreModule/nodes/types/buttonType.h"
#include "generic/coreModule/nodes/types/eventNode.h"

namespace bt::interfaceModule {

    class controllerWidget
      : public generic::coreModule::buttonType<cocos2d::Node>
      , public generic::coreModule::nodeProperties {
    public:
        controllerWidget();
    };
}// namespace bt::interfaceModule


#endif// BENTO_TIME_CONTROLLERWIDGET_H
