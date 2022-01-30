#ifndef BENTO_TIME_GAMEBOARD_H
#define BENTO_TIME_GAMEBOARD_H

#include "cocos2d.h"
#include "databaseModule/levelsTool.h"
#include "gameplayModule/bentoNode.h"
#include "generic/coreModule/nodes/nodeProperties.h"
#include "generic/coreModule/signals/signalHolder.h"
#include "interfaceModule/widgets/controllerWidget.h"
#include <functional>
#include <string>
#include <tuple>
#include <vector>


namespace bt::gameplayModule {

    struct battleBoardEvents {
        generic::signal::signalHolder<> onPlayerMove;
        generic::signal::signalHolder<> onPlayerConnect;
    };

    class gameBoard
      : public generic::coreModule::nodeProperties
      , public cocos2d::Layer {
    public:
        gameBoard();
        void loadLevel(int id);
        ~gameBoard() override;
        const std::map<int, std::map<int, bentoNode*>>& getTiles() {
            return tilesOnMap;
        };
        cocos2d::TMXTiledMap* getTiled() const {
            return tiledMap;
        }

        battleBoardEvents* getEmitter() {
            return &eventHolder;
        }

        void attachController(const interfaceModule::sControllerEvents& emitter);

    private:
        void reloadWalls(const databaseModule::sLevelData&);
        void spawnObjects(int id);
        battleBoardEvents eventHolder;
        cocos2d::Node* gameFieldNode = nullptr;
        cocos2d::Node* objectsLayer = nullptr;
        int currentLevelId = -1;

        // tiled
        cocos2d::TMXTiledMap* tiledMap = nullptr;
        databaseModule::levelsTool levelTool;
        // (x, y from tiled) -> Node*
        std::map<int, std::map<int, bentoNode*>> tilesOnMap;

        // walls
        // (x, y from tiled) -> [wallType]
        std::map<int, std::map<int, std::vector<databaseModule::eLocationWallType>>> wallOnMap;
    };
}// namespace bt::gameplayModule


#endif// BENTO_TIME_GAMEBOARD_H
