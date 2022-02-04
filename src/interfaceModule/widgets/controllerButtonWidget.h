#ifndef BENTO_TIME_CONTROLLERBUTTONWIDGET_H
#define BENTO_TIME_CONTROLLERBUTTONWIDGET_H

#include "cocos2d.h"
#include "generic/coreModule/nodes/nodeProperties.h"
#include "generic/coreModule/nodes/types/buttonType.h"
#include "generic/coreModule/nodes/types/eventNode.h"
#include "generic/coreModule/nodes/types/asepriteNode.h"
#include <functional>


namespace bt::interfaceModule {

    enum class eControllerButtonType { TYPE_A = 0, TYPE_B };
    enum class eControllerIconType { MENU = 0, REPLAY };

    class controllerButtonWidget
      : public generic::coreModule::buttonType<cocos2d::Node>
      , public generic::coreModule::nodeProperties {
    public:
        controllerButtonWidget();
        void updateSettings() override;
        eventTouchClb getOnTouchBegan() override;
        eventTouchClb getOnTouchEnded() override;

    private:
        void updateButton(eControllerButtonType buttonType, eControllerIconType iconType, const std::string& iconAnimation = "");

        generic::coreModule::asepriteNode* btnNode = nullptr;
        generic::coreModule::asepriteNode* iconNode = nullptr;

    };
}// namespace bt::interfaceModule


#endif// BENTO_TIME_CONTROLLERBUTTONWIDGET_H
