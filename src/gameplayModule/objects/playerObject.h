#ifndef BENTO_TIME_PLAYEROBJECT_H
#define BENTO_TIME_PLAYEROBJECT_H

#include "cocos2d.h"
#include "generic/coreModule/nodes/nodeProperties.h"
#include "gameplayModule/objects/objectOnMap.h"
#include <string>

namespace bt::gameplayModule {
    class playerObject
      : public generic::coreModule::nodeProperties
      , public objectOnMap {
    public:
        playerObject();
        ~playerObject() override;
        CREATE_FUNC(playerObject);
        void initWithData(const bt::databaseModule::sMapObjectsData&) override;

    };
}// namespace bt::gameplayModule


#endif// BENTO_TIME_PLAYEROBJECT_H
