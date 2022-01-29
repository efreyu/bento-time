#ifndef BENTO_TIME_BUTTONWIDGET_H
#define BENTO_TIME_BUTTONWIDGET_H

#include "cocos2d.h"
#include "generic/coreModule/nodes/nodeProperties.h"
#include "generic/coreModule/nodes/types/soundButton.h"

namespace bt::interfaceModule {

    class buttonWidget : public generic::coreModule::soundButton {
    public:
        buttonWidget();
        ~buttonWidget() override = default;
        void updateSettings() override;

        void setText(const std::string&);

    private:
        cocos2d::Label* label = nullptr;
    };
}// namespace bt::interfaceModule

#endif// BENTO_TIME_BUTTONWIDGET_H
