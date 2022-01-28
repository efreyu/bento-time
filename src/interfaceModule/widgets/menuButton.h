#ifndef BENTO_TIME_MENUBUTTON_H
#define BENTO_TIME_MENUBUTTON_H

#include "cocos2d.h"
#include "generic/coreModule/nodes/nodeProperties.h"
#include "generic/coreModule/nodes/types/soundButton.h"

namespace bt::interfaceModule {

    class menuButton
      : public generic::coreModule::soundButton {
    public:
        menuButton();
        ~menuButton() override;
        CREATE_FUNC(menuButton);

    };

}// namespace bt::interfaceModule


#endif// BENTO_TIME_MENUBUTTON_H
