#ifndef BENTO_TIME_MAPDISPATCHER_H
#define BENTO_TIME_MAPDISPATCHER_H

#include "cocos2d.h"
#include "moveEnum.h"
#include "databaseModule/levelsTool.h"
#include <map>
#include <set>
#include <tuple>

namespace bt::gameplayModule {

    class bentoNode;

    std::pair<bentoNode*, std::set<databaseModule::eLocationWallType>> typedef mapCell;

    class mapDispatcher {
    public:
        ~mapDispatcher();
        static mapDispatcher* createWithObjectsNode(cocos2d::Node* node, cocos2d::TMXTiledMap* tiled, int levelId);
        bool move(eMoveDirection);
    private:
        void setObjectNode(cocos2d::Node* node) { objectsNode = node; }
        void loadWalls(const databaseModule::sLevelData&, cocos2d::TMXTiledMap* tiled);
        void spawnObjects(const databaseModule::sLevelData&, cocos2d::TMXTiledMap* tiled);
        mapDispatcher() = default;

        databaseModule::levelsTool levelTool;

        // (x, y from tiled) -> cell
        std::map<int, std::map<int, mapCell>> cells;

    protected:
        cocos2d::Node* objectsNode = nullptr;
    };
}


#endif// BENTO_TIME_MAPDISPATCHER_H
