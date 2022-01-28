#pragma once
#ifndef BENTO_TIME_ROLLBACKBUTTON_H
#define BENTO_TIME_ROLLBACKBUTTON_H

#include "cocos2d.h"
#include "generic/coreModule/nodes/nodeProperties.h"
#include "generic/coreModule/nodes/types/soundButton.h"

namespace bt::interfaceModule {

    class rollbackButton
      : public generic::coreModule::soundButton {
    public:
        rollbackButton();
        ~rollbackButton() override;
        CREATE_FUNC(rollbackButton);

        cocos2d::Label* getLabel();
        void setLabelText(const std::string& str = std::string());

    };

}// namespace bt::interfaceModule


#endif// BENTO_TIME_ROLLBACKBUTTON_H
