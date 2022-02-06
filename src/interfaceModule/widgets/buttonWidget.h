#ifndef BENTO_TIME_BUTTONWIDGET_H
#define BENTO_TIME_BUTTONWIDGET_H

#include "cocos2d.h"
#include "generic/coreModule/nodes/nodeProperties.h"

namespace bt::interfaceModule {

    class buttonWidget
      : public cocos2d::Node
      , public generic::coreModule::nodeProperties {
    public:
        buttonWidget();

        void setText(const std::string&);
        void setDisabled();
        void setSelect(bool status);
        void setSmallText();

    private:
        void initWidget();
        float selectHorizontalOffset = 0.f;
    };
}// namespace bt::interfaceModule

#endif// BENTO_TIME_BUTTONWIDGET_H
