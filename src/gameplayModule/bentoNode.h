#ifndef BENTO_TIME_BENTONODE_H
#define BENTO_TIME_BENTONODE_H

#include "cocos2d.h"
#include "databaseModule/mapObjectsDatabase.h"
#include "generic/coreModule/nodes/nodeProperties.h"
#include <string>

namespace bt::gameplayModule {
    class bentoNode
      : public generic::coreModule::nodeProperties
      , public cocos2d::Node {
    public:
        bentoNode();
        ~bentoNode() override;
        CREATE_FUNC(bentoNode);
        void initWithData(const bt::databaseModule::sMapObjectsData&);
        databaseModule::eMapObjectType type;
        int objectId = 0;// local id from database
    };
}// namespace bt::gameplayModule


#endif// BENTO_TIME_BENTONODE_H
