#ifndef BENTO_TIME_GAMEBOARD_H
#define BENTO_TIME_GAMEBOARD_H

#include "cocos2d.h"
#include "generic/coreModule/nodes/nodeProperties.h"
#include "generic/coreModule/signals/signalHolder.h"
#include "moveEnum.h"
#include <functional>
#include <string>
#include <tuple>
#include <vector>


namespace bt::interfaceModule {
    class sControllerEvents;
}

namespace bt::gameplayModule {

    class mapDispatcher;

    class gameBoard
      : public generic::coreModule::nodeProperties
      , public cocos2d::Layer {
    public:
        gameBoard();
        void loadLevel(int id);
        ~gameBoard() override;

        cocos2d::TMXTiledMap* getTiled() const {
            return tiledMap;
        }

        void attachController(interfaceModule::sControllerEvents* emitter);

    private:
        cocos2d::Node* gameFieldNode = nullptr;
        mapDispatcher* dispatcher = nullptr;
        cocos2d::TMXTiledMap* tiledMap = nullptr;
        int currentLevel;
    };
}// namespace bt::gameplayModule


#endif// BENTO_TIME_GAMEBOARD_H
