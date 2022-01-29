#ifndef BENTO_TIME_OBJECTONMAP_H
#define BENTO_TIME_OBJECTONMAP_H

#include "databaseModule/mapObjectsDatabase.h"
#include "cocos2d.h"

namespace bt::gameplayModule {

    class objectOnMap : public cocos2d::Node {
    public:
        virtual ~objectOnMap() {};

        virtual void initWithData(const bt::databaseModule::sMapObjectsData&) = 0;

        databaseModule::eMapObjectType type;
        int objectId = 0;// local id from database
    };

}// namespace bt::gameplayModule


#endif// BENTO_TIME_OBJECTONMAP_H
