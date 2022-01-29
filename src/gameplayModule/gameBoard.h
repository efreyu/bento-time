#ifndef BENTO_TIME_GAMEBOARD_H
#define BENTO_TIME_GAMEBOARD_H

#include "gameplayModule/objects/objectOnMap.h"
#include "cocos2d.h"
#include "databaseModule/levelsTool.h"
#include "generic/coreModule/nodes/nodeProperties.h"
#include "generic/coreModule/signals/signalHolder.h"
#include <string>
#include <tuple>
#include <vector>
#include <functional>


namespace bt::gameplayModule {

    struct battleBoardEvents {
        generic::signal::signalHolder<bool> onPlayerMove;
    };

    class gameBoard
      : public generic::coreModule::nodeProperties
      , public cocos2d::Layer {
    public:
        gameBoard();
        void loadLevel(int id);
        ~gameBoard() override;
        const std::map<int, std::map<int, objectOnMap*>>& getTiles() { return tilesOnMap; };
        cocos2d::TMXTiledMap* getTiled() const { return tiledMap; }

        battleBoardEvents* getEmitter() { return &eventHolder; }

    private:
        void reloadWalls(const databaseModule::sLevelData&);
        void spawnObjects(int id);
        battleBoardEvents eventHolder;
        cocos2d::Node* gameFieldNode = nullptr;
        cocos2d::Node* objectsLayer = nullptr; //todo not used
        int currentLevelId = -1;

        // tiled
        cocos2d::TMXTiledMap* tiledMap = nullptr;
        databaseModule::levelsTool levelTool;
        // (x, y from tiled) -> Node*
        std::map<int, std::map<int, objectOnMap*>> tilesOnMap;

        // walls
        // (x, y from tiled) -> [wallType]
        std::map<int, std::map<int, std::vector<databaseModule::eLocationWallType>>> wallOnMap;
    };
}// namespace bt::gameplayModule


#endif// BENTO_TIME_GAMEBOARD_H
