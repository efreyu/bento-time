#ifndef BENTO_TIME_CONTROLLERWIDGET_H
#define BENTO_TIME_CONTROLLERWIDGET_H

#include "cocos2d.h"
#include "generic/coreModule/nodes/nodeProperties.h"
#include "generic/coreModule/nodes/types/asepriteNode.h"
#include "generic/coreModule/nodes/types/buttonType.h"
#include "generic/coreModule/nodes/types/eventNode.h"
#include "generic/coreModule/signals/signalHolder.h"

namespace bt::interfaceModule {

    enum class eMoveDirection {
        UNDEFINED = 0,
        UP,
        DOWN,
        LEFT,
        RIGHT
    };

    struct sControllerEvents {
        generic::signal::signalHolder<eMoveDirection> onPressed;
        generic::signal::signalHolder<> onReleased;
    };

    class controllerWidget
      : public generic::coreModule::buttonType<cocos2d::Node>
      , public generic::coreModule::nodeProperties {
    public:
        controllerWidget();
        bool init() override;

        sControllerEvents* getEmitter() {
            return &eventHolder;
        }

    private:
        void initHandler();
        bool touchProceed(cocos2d::Touch* touch, cocos2d::Event* event);
        void onButtonHold();

        cocos2d::EventListenerTouchOneByOne* listener = nullptr;
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32) || (CC_TARGET_PLATFORM == CC_PLATFORM_MAC) || (CC_TARGET_PLATFORM == CC_PLATFORM_LINUX)
        cocos2d::EventListenerKeyboard* keyboardListener = nullptr;
#endif
        std::map<eMoveDirection, cocos2d::Node*> nodesWithDirections;
        eMoveDirection currentPressed = eMoveDirection::UNDEFINED;
        generic::coreModule::asepriteNode* arrowsNode = nullptr;
        sControllerEvents eventHolder;
    };
}// namespace bt::interfaceModule


#endif// BENTO_TIME_CONTROLLERWIDGET_H
