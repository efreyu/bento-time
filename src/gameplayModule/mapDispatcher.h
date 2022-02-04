#ifndef BENTO_TIME_MAPDISPATCHER_H
#define BENTO_TIME_MAPDISPATCHER_H

#include "cocos2d.h"
#include "databaseModule/levelsTool.h"
#include "generic/coreModule/signals/signalHolder.h"
#include "moveEnum.h"
#include <functional>
#include <map>
#include <memory>
#include <set>
#include <tuple>

namespace bt::gameplayModule {

    class bentoNode;
    struct mapCellItems;

    typedef std::function<bool(eMoveDirection, const std::pair<int, int>&)> moveClbType;
    typedef std::shared_ptr<mapCellItems> mapCellItemsPtr;

    struct mapCell {
        bentoNode* node = nullptr;
        std::pair<int, int> pos;
        bool moved = false;
        mapCell(bentoNode* _node, const std::pair<int, int>& _pos) : node(_node), pos(_pos){};
        bool move(eMoveDirection direction);
        void registerMoveClb(const moveClbType& clb) {
            moveClb = clb;
        };
        bool operator()(const mapCell& c) const {
            return pos == c.pos;
        }

    private:
        moveClbType moveClb = nullptr;
    };

    struct mapCellItems {
        mapCell* cell = nullptr;
        std::map<eMoveDirection, mapCellItemsPtr> cells;
        mapCellItems(mapCell* _cell) : cell(_cell) {}
        bool move(eMoveDirection);
        void reset();
    };

    struct mapDispatcherEvents {
        generic::signal::signalHolder<> onPlayerMove;
        generic::signal::signalHolder<> onWin;
    };

    class mapDispatcher {
    public:
        ~mapDispatcher();
        static mapDispatcher* createWithObjectsNode(cocos2d::Node* node, cocos2d::TMXTiledMap* tiled, int levelId);
        bool move(eMoveDirection);
        mapDispatcherEvents* getEmitter() {
            return &eventHolder;
        }

    private:
        mapDispatcher() = default;
        void setObjectNode(cocos2d::Node* node) {
            objectsNode = node;
        }
        void loadWalls(const databaseModule::sLevelData&, cocos2d::TMXTiledMap* tiled);
        void spawnObjects(const databaseModule::sLevelData&, cocos2d::TMXTiledMap* tiled);
        std::pair<int, int> getNextIndex(eMoveDirection direction, const std::pair<int, int>& nextPosition);
        bool hasCollision(eMoveDirection direction, const std::pair<int, int>& currentPos);
        mapCell* getCellByPos(const std::pair<int, int>& pos);
        bool moveCell(eMoveDirection direction, const std::pair<int, int>& currentPos);
        void getClosestCells(mapCellItemsPtr& closest, std::vector<mapCell*>& exclude, eMoveDirection dir);


        databaseModule::levelsTool levelTool;
        cocos2d::Node* objectsNode = nullptr;
        std::pair<int, int> mapSize;
        mapDispatcherEvents eventHolder;

        // (x, y from tiled)
        std::map<int, std::map<int, mapCell*>> cells;
        std::map<int, std::map<int, std::set<databaseModule::eLocationWallType>>> walls;
        std::vector<mapCell*> playerCells;
        std::vector<std::pair<int, int>> exitCells;
    };
}// namespace bt::gameplayModule


#endif// BENTO_TIME_MAPDISPATCHER_H
