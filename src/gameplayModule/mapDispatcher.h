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

    struct mapCell {
        bentoNode* node = nullptr;
        bool connected = false;
        std::pair<int, int> pos;
        mapCell(bentoNode* _node, std::pair<int, int> _pos) : node(_node), pos(_pos) {};
    };

    class mapDispatcher {
    public:
        ~mapDispatcher();
        static mapDispatcher* createWithObjectsNode(cocos2d::Node* node, cocos2d::TMXTiledMap* tiled, int levelId);
        bool move(eMoveDirection);
    private:
        mapDispatcher() = default;
        void setObjectNode(cocos2d::Node* node) { objectsNode = node; }
        void loadWalls(const databaseModule::sLevelData&, cocos2d::TMXTiledMap* tiled);
        void spawnObjects(const databaseModule::sLevelData&, cocos2d::TMXTiledMap* tiled);
        void updateNextCell(eMoveDirection direction, std::pair<int, int>& nextPosition);
        cocos2d::Vec2 getNextPosition(eMoveDirection direction, cocos2d::Vec2 pos, const cocos2d::Size& size, float scale);

        databaseModule::levelsTool levelTool;

        std::vector<mapCell*> cells;
        // (x, y from tiled) -> walls
        std::map<int, std::map<int, std::set<databaseModule::eLocationWallType>>> walls;

    protected:
        cocos2d::Node* objectsNode = nullptr;
        std::pair<unsigned, unsigned> mapSize;
    };
}


#endif// BENTO_TIME_MAPDISPATCHER_H
