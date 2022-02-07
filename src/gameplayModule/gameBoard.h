#ifndef BENTO_TIME_GAMEBOARD_H
#define BENTO_TIME_GAMEBOARD_H

#include "cocos2d.h"
#include "generic/coreModule/nodes/nodeProperties.h"
#include "generic/coreModule/nodes/types/eventNode.h"
#include "generic/coreModule/signals/signalHolder.h"
#include "moveEnum.h"
#include <functional>
#include <string>
#include <tuple>
#include <vector>


namespace bt::interfaceModule {
    struct sControllerStickEvents;
}

namespace bt::gameplayModule {

    class mapDispatcher;

    class gameBoard
      : public generic::coreModule::nodeProperties
      , public cocos2d::Node {
    public:
        gameBoard();
        void loadLevel(int id);
        ~gameBoard() override;
        cocos2d::TMXTiledMap* getTiled() const {
            return tiledMap;
        }
        void attachController(interfaceModule::sControllerStickEvents* emitter, generic::coreModule::eventNode* replayBtn);

    private:
        void loadSettings();
        void updateMovesScore();
        void runShowAnimation(const std::function<void()>& clb);
        void runHideAnimation(const std::function<void()>& clb);

        cocos2d::Node* gameFieldNode = nullptr;
        mapDispatcher* dispatcher = nullptr;
        cocos2d::TMXTiledMap* tiledMap = nullptr;
        cocos2d::Label* movesLabel = nullptr;
        cocos2d::Label* levelsLabel = nullptr;
        int currentLevel;
        int movesCnt;
        bool boardBlocked = true;

        struct gameBoardSettings {
            std::string movesPattern;
            std::string levelsPattern;
            float fadeDuration;
            float delayDuration;
        };
        gameBoardSettings settings;
    };
}// namespace bt::gameplayModule


#endif// BENTO_TIME_GAMEBOARD_H
